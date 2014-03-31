#include "../include/CouchFine.h"


namespace CouchFine {


Database& operator>>(
    Database& store,
    Object& doc
) {
    const std::string& uid = boost::any_cast< std::string >( *doc[ "_id" ] );
    if ( uid.empty() ) {
        std::cerr << "ID required." << std::endl;
        throw Exception( "ID required." );
    }

    // �� ������
    try {
        // @todo optimize ������ �� ������� ���� ��������: ���������� parseData() 2 ����.
        const Document d = store.getDocument( uid );
        const auto& t = d.getData();
        doc = boost::any_cast< Object >( *t );

    } catch ( ... ) {
        // ������ �� �����, ������ ��������
        doc = Object();
    }

    return store;
}




Database& operator>>(
    Database& store,
    Mode::Doc& doc
) {
    // ���������� ������ CouchDB ������� ��� ��������� � ��������� UID
    // @see http://wiki.apache.org/couchdb/HTTP_view_API#Querying_Options / keys

    // �������� ��������� �� ���������
    // ��������� POST-������ � ������� JSON
    // @source http://wiki.apache.org/couchdb/HTTP_Bulk_Document_API
    const std::string url = "/" + store.getName() + "/_all_docs?include_docs=true";
    std::ostringstream ss;
    ss << "{\"keys\":";
        typelib::print( ss, doc.uid, "[", "]", "\"", "," );
    ss << "}";

    // �� ������
    doc.ok = true;
    try {
        typelib::json::Variant var =
            store.getCommunication().getData( url, "POST", ss.str() );
        typelib::json::Object o = static_cast< typelib::json::Object >( var );
        // ����� ����� ���-�� ���� ���������� � ���������
        doc.totalRows = static_cast< size_t >( o["total_rows"] );
        // ����� - ��, ������� �� ��������� � �� ����� 'keys'
        doc.result = static_cast< Array >( o["rows"] );

    } catch ( const Exception& ex ) {
        // ������ ������ � ������� �� ������
        doc.totalRows = 0;
        doc.result = Array();
        doc.ok = false;
        doc.exception = std::shared_ptr< Exception >( new Exception( ex ) );
    }

    return store;
}






Database& operator>>(
    Database& store,
    Mode::View& view
) {
    /* - @todo ����� �������� �� ����� ������� �� http://wiki.apache.org/couchdb/HTTP_view_API?action=show&redirect=HttpViewApi#Querying_Options
    // ����� ������ ���������� ����� �����
    std::string key = "";
    for (auto itr = view.key.cbegin(); itr != view.key.cend(); ++itr) {
        const std::string delimiter =
            (std::distance( view.key.cbegin(), itr) == 0) ? "?" : "&";
        key += delimiter + itr->first + "=" + itr->second;
    }
    */

    std::string key = view.key;
    if ( view.withDoc ) {
        key += "&include_docs=true";
    }
    if (view.limit > 0) {
        key += "&limit=" + boost::lexical_cast< std::string >( view.limit );
    }
    if (key[0] == '&') {
        key = key.substr( 1 );
    }

    // �� ������
    view.ok = true;
    try {
        Object o = store.getView( view.view, view.design, key );
        // (!) ��� ������������� ������ limit / offset, �������� 'totalRows'
        // �� ��������� � 'result.count()'
        view.totalRows = static_cast< size_t >( o["total_rows"] );
        view.result = static_cast< Array >( o["rows"] );

    } catch ( const Exception& ex ) {
        // ������ ������ � ������� �� ������
        view.totalRows = 0;
        view.result = Array();
        view.ok = false;
        view.exception = std::shared_ptr< Exception >( new Exception( ex ) );
    }

    return store;
}






Database& operator<<(
    Database& store,
    Mode::NewOnly& doc
) {
    // 'doc' ����� ���� ����������� ��� Object ��� ��� Pool
    assert( doc.p || doc.o );

    if ( doc.p ) {
        const Array a = (Array)( *doc.p );
        store.createBulk( a, doc.fnCreateJSON );

    } else {
        store.createDocument( typelib::json::cjv( *doc.o ) );
    }

    return store;
}






Database& operator<<(
    Database& store,
    Mode::NewSkip& doc
) {
    // @info ���������� NewUpdate, �� ����� � �������� �������

    // 'doc' ����� ���� ����������� ��� Object ��� ��� Pool
    assert( doc.p || doc.o );

    if ( doc.p ) {
        // �������� ������, �. � �������� ������� - ���������
        const Array a = (Array)( *doc.p );
        const Array r = store.createBulk( a, doc.fnCreateJSON );
        // ������ ����������: ����������� ������ ��� "������������ �������"
        // � ������������ ����������
        // @todo fine ������������� ������������ ��� ������?

    } else {
        // ���������������� ������ ���� �� - ��� �����
        Pool singleRepush;
        singleRepush.push_back( typelib::json::cjv( *doc.o ) );
        store << Mode::NewSkip( singleRepush, doc.fnCreateJSON );

    } // else if ( doc.p )

    return store;
}






Database& operator<<(
    Database& store,
    Mode::File& file
) {
    assert( !file.uidDoc.empty()
        && "����������� UID ���������, � �������� ������� ���������� ����." );

    Document doc = store.createDocument( Object(), file.uidDoc );
    if ( !file.revisionDoc.empty() ) {
        doc.setRevision( file.revisionDoc );
    }

    const bool result =
        doc.addAttachment( file.name, "text/plain", file.data );
    if ( !result ) {
        throw Exception( "Could not create attachment '" + file.name + "' with data '" + file.data + "'." );
    }

    return store;
}






Database& operator<<(
    Database& store,
    Mode::NewUpdate& doc
) {
    assert( (doc.p || doc.o)
        && "'doc' ������ ���� ����������� ��� ��� Object, ��� ��� Pool." );

    if ( doc.p ) {
        // *Pool*
        // ��������� ��������� � �����-��������
        // �������� ������; �. � �������� ������� - �������
        Array& a = static_cast< Array& >( *doc.p );
        // #! @todo ����� ������������� ������?
        //    1. doc �� ������ ���� const.
        //    2. doc ������ �������� �� typelib::json::variant.
        const Array result = store.createBulk( a, doc.fnCreateJSON );
        // ������� ������, ��������� UID � REV, �������� ���������
        // ��� ���������� ������ � ���������
        Pool repush;
        for (auto rtr = result.cbegin(); rtr != result.cend(); ++rtr) {
            //const auto& type = itr->get()->type();

            // ��������� createBulk() � �������� �����. ������� ������������ ���������
            const auto i = std::distance( result.cbegin(), rtr );
            auto& objAny = *a.at( i );
#ifdef _DEBUG
            const std::string& typeName = objAny.type().name();
#endif
            /* - ���������� Pool �� ����� ���� � ������ std::shared_ptr.
            Object* obj =
                (objAny.type() == typeid( std::shared_ptr< Object > ))
                ? boost::any_cast< std::shared_ptr< Object > >( objAny ).get()
                : boost::any_cast< Object* >( objAny );
            */
            assert( (objAny.type() == typeid( Object* ))
                && "�������� Object �������� �� �� ������. ������� ������ ���������������� ��������. ����������� ������ std::shared_ptr ��� �������� Object � Pool." );
            Object* obj = boost::any_cast< Object* >( objAny );

            const Object& ro = boost::any_cast< Object >( **rtr );
            if ( !hasError( ro ) ) {
                // ���������� �������� �������� UID � �������
                const uid_t& uid = CouchFine::uid( ro );
                const rev_t& rev = revision( ro );
                CouchFine::uid( *obj, uid, rev );
                // 'obj', ���, �� ��������� �� �������� �� 'doc.p'
                //a.at( i ) = typelib::json::cjv( obj );
                // @todo fine � ���� �������� ������ ����� ���������� ������? ��������
                //       � ��� �������� ������������ ������� ������� ������.
                continue;
            }

            // ��������� �������� (��� �������) ���� ����� ��������� ��������
#ifdef _DEBUG
            std::cerr << error( ro ) << std::endl;
#endif

            // ������� ������� ������������� ���������
            const auto uid = CouchFine::uid( *obj );
            assert( !uid.empty()
                && "��� ��������� ��� UID ������� �� ����� ���� ��������." );
            Object sObj;
            store >> CouchFine::uid( sObj, uid );
            if ( hasError( sObj ) ) {
                // ������ �. � ��������� ��� ��� ?!
                std::cerr << "operator<<( Database, NewUpdate ) " << error( sObj ) << std::endl;
                throw Exception( "Unrecognized exception: " + error( sObj ) );
            }

            // �. � �������� � 'doc' UID ���������� � ���������, ������� ���
            const rev_t& rev = revision( sObj );
            revision( *obj, rev );
            repush << obj;

        } // for (auto itr = r.cbegin(); itr != r.cend(); ++itr)

        // ��������� �������� (���������)
        if ( !repush.empty() ) {
            store << Mode::NewUpdate( repush, doc.fnCreateJSON );
            // @todo fine ����������� ����������� �����.
        }

    } else {
        // *Object*
        // ���������������� ������ ���� �� - ��� �����...
        Pool singleRepush;
        singleRepush.push_back( typelib::json::cjv( *doc.o ) );
        store << Mode::NewUpdate( singleRepush, doc.fnCreateJSON );

        // ...�� ������: ���� ���������� �������
            const auto& objAny = *singleRepush.front();
            const Object& ro = boost::any_cast< Object >( objAny );
            if ( hasError( ro ) ) {
                std::cerr << "operator<<( Database, NewUpdate ) " << error( ro ) << std::endl;
                throw Exception( "Unrecognized exception: " + error( ro ) );
            }
            // ���������� �������� �������� UID � �������
            const uid_t& uid = CouchFine::uid( ro );
            const rev_t& rev = revision( ro );
            CouchFine::uid( *doc.o, uid, rev );
            // @todo fine � ���� �������� ������ ����� ���������� ������? ��������
            //       � ��� �������� ������������ ������� ������� ������.

    } // else if ( doc.p )

    return store;
}



} // CouchFine
