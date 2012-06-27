#pragma once

// @todo fine ������ using namespace �� global.

#include "configure.h"
#include "Mode.h"
#include "Communication.h"
#include "Connection.h"
#include "View.h"
#include "Database.h"
#include "Pool.h"


/**
* (!) ������ design-������� ������ �������������� ������ ��������� �������.
*/


// ������ ������ � �������� �������� ����, �����:
// 1. ���� ���������.
// 2. �� ����� ���� ���������� ��� inline.


/**
* ������ ���������� �� ���������.
*/

inline CouchFine::Database& operator>>(
    CouchFine::Database& store,
    CouchFine::Object& doc
) {
    const std::string& uid = boost::any_cast< std::string >( *doc[ "_id" ] );
    if ( uid.empty() ) {
        std::cerr << "ID required." << std::endl;
        throw CouchFine::Exception( "ID required." );
    }

    // �� ������
    try {
        // @todo optimize ������ �� ������� ���� ��������: ���������� parseData() 2 ����.
        const CouchFine::Document d = store.getDocument( uid );
        const auto& t = d.getData();
        doc = boost::any_cast< CouchFine::Object >( *t );

    } catch ( ... ) {
        // ������ �� �����, ������ ��������
        doc = CouchFine::Object();
    }

    return store;
}




inline CouchFine::Database& operator>>(
    CouchFine::Database& store,
    CouchFine::Mode::View& view
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

    const std::string key = view.key;
    std::string otherKey = view.withDoc ? "include_docs=true" : "";
    otherKey += (view.limit > 0)
        ? ( (otherKey.empty() ? "" : "&") + ("limit=" + boost::lexical_cast< std::string >( view.limit ))  )
        : "";

    // �� ������
    view.ok = true;
    try {
        CouchFine::Object o = store.getView( view.view, view.design, key, otherKey );
        view.result = boost::any_cast< CouchFine::Array >( *o["rows"] );

    } catch ( const CouchFine::Exception& ex ) {
        // ������ ������ � ������� �� ������
        view.result = CouchFine::Array();
        view.ok = false;
        view.exception = std::shared_ptr< CouchFine::Exception >( new CouchFine::Exception( ex ) );
    }

    return store;
}








/**
* ���������� ���������� � ���������.
*/


/**
* ��������� �������� � CouchDB.
*//* - �� ������������.
inline CouchFine::Database& operator<<(
    CouchFine::Database& store,
    const CouchFine::Variant& doc
) {
    store.createDocument( doc );
    return store;
}
*/


// ����������� ���� �����, ����� ��������� ������� �������������� �����.
template< typename T >
inline CouchFine::Database& operator<<(
    CouchFine::Database& store,
    T& data
) {
    static_assert( (typename( T ) == typename( CouchFine::Object )),
        "����������� � CouchFine::Mode::*() ��� ������ ������ � ����������� � ���������." );
    static_assert( false, "��� �� ��������������." );
    return store;
}




inline CouchFine::Database& operator<<(
    CouchFine::Database& store,
    CouchFine::Mode::NewOnly& doc
) {
    // 'doc' ����� ���� ����������� ��� Object ��� ��� Pool
    assert( doc.p || doc.o );

    if ( doc.p ) {
        const CouchFine::Array a = (CouchFine::Array)( *doc.p );
        store.createBulk( a, doc.fnCreateJSON );

    } else {
        store.createDocument( typelib::json::cjv( *doc.o ) );
    }

    return store;
}






inline CouchFine::Database& operator<<(
    CouchFine::Database& store,
    CouchFine::Mode::NewSkip& doc
) {
    // @info ���������� NewUpdate, �� ����� � �������� �������

    // 'doc' ����� ���� ����������� ��� Object ��� ��� Pool
    assert( doc.p || doc.o );

    if ( doc.p ) {
        // �������� ������, �. � �������� ������� - ���������
        const CouchFine::Array a = (CouchFine::Array)( *doc.p );
        const CouchFine::Array r = store.createBulk( a, doc.fnCreateJSON );
        // ������ ����������: ����������� ������ ��� "������������ �������"
        // � ������������ ����������
        // @todo fine ������������� ������������ ��� ������?

    } else {
        // ���������������� ������ ���� �� - ��� �����
        CouchFine::Pool singleRepush;
        singleRepush.push_back( typelib::json::cjv( *doc.o ) );
        store << CouchFine::Mode::NewSkip( singleRepush, doc.fnCreateJSON );

    } // else if ( doc.p )

    return store;
}






inline CouchFine::Database& operator<<(
    CouchFine::Database& store,
    CouchFine::Mode::File& file
) {
    assert( !file.uidDoc.empty()
        && "����������� UID ���������, � �������� ������� ���������� ����." );

    CouchFine::Document doc = store.createDocument( CouchFine::Object(), file.uidDoc );
    if ( !file.revisionDoc.empty() ) {
        doc.setRevision( file.revisionDoc );
    }

    const bool result =
        doc.addAttachment( file.name, "text/plain", file.data );
    if ( !result ) {
        throw CouchFine::Exception( "Could not create attachment '" + file.name + "' with data '" + file.data + "'." );
    }
}






inline CouchFine::Database& operator<<(
    CouchFine::Database& store,
    CouchFine::Mode::NewUpdate& doc
) {
    assert( (doc.p || doc.o)
        && "'doc' ������ ���� ����������� ��� ��� Object, ��� ��� Pool." );

    if ( doc.p ) {
        // *Pool*
        // ��������� ��������� � �����-��������
        // �������� ������; �. � �������� ������� - �������
        CouchFine::Array& a = static_cast< CouchFine::Array& >( *doc.p );
        const CouchFine::Array result = store.createBulk( a, doc.fnCreateJSON );
        // ������� ������, ��������� UID � REV, �������� ���������
        // ��� ���������� ������ � ���������
        CouchFine::Pool repush;
        for (auto rtr = result.cbegin(); rtr != result.cend(); ++rtr) {
            //const auto& type = itr->get()->type();

            // ��������� createBulk() � �������� �����. ������� ������������ ���������
            const auto i = std::distance( result.cbegin(), rtr );
            auto& objAny = *a.at( i );
#ifdef _DEBUG
            const std::string& typeName = objAny.type().name();
#endif
            /* - ���������� Pool �� ����� ���� � ������ std::shared_ptr.
            CouchFine::Object* obj =
                (objAny.type() == typeid( std::shared_ptr< CouchFine::Object > ))
                ? boost::any_cast< std::shared_ptr< CouchFine::Object > >( objAny ).get()
                : boost::any_cast< CouchFine::Object* >( objAny );
            */
            assert( (objAny.type() == typeid( CouchFine::Object* ))
                && "�������� Object �������� �� �� ������. ������� ������ ���������������� ��������. ����������� ������ std::shared_ptr ��� �������� Object � Pool." );
            CouchFine::Object* obj = boost::any_cast< CouchFine::Object* >( objAny );

            const CouchFine::Object& ro = boost::any_cast< CouchFine::Object >( **rtr );
            if ( !CouchFine::hasError( ro ) ) {
                // ���������� �������� �������� UID � �������
                const CouchFine::uid_t& uid = CouchFine::uid( ro );
                const CouchFine::rev_t& rev = CouchFine::revision( ro );
                CouchFine::uid( *obj, uid, rev );
                // 'obj', ���, �� ��������� �� �������� �� 'doc.p'
                //a.at( i ) = typelib::json::cjv( obj );
                // @todo fine � ���� �������� ������ ����� ���������� ������? ��������
                //       � ��� �������� ������������ ������� ������� ������.
                continue;
            }

            // ��������� �������� (��� �������) ���� ����� ��������� ��������
#ifdef _DEBUG
            std::cerr << CouchFine::error( ro ) << std::endl;
#endif

            // ������� ������� ������������� ���������
            const auto uid = CouchFine::uid( *obj );
            assert( !uid.empty()
                && "��� ��������� ��� UID ������� �� ����� ���� ��������." );
            CouchFine::Object sObj;
            store >> CouchFine::uid( sObj, uid );
            if ( CouchFine::hasError( sObj ) ) {
                // ������ �. � ��������� ��� ��� ?!
                std::cerr << "CouchFine::operator<<( Database, NewUpdate ) " << CouchFine::error( sObj ) << std::endl;
                throw CouchFine::Exception( "Unrecognized exception: " + CouchFine::error( sObj ) );
            }

            // �. � �������� � 'doc' UID ���������� � ���������, ������� ���
            const CouchFine::rev_t& rev = CouchFine::revision( sObj );
            CouchFine::revision( *obj, rev );
            repush << obj;

        } // for (auto itr = r.cbegin(); itr != r.cend(); ++itr)

        // ��������� �������� (���������)
        if ( !repush.empty() ) {
            store << CouchFine::Mode::NewUpdate( repush, doc.fnCreateJSON );
            // @todo fine ����������� ����������� �����.
        }

    } else {
        // *Object*
        // ���������������� ������ ���� �� - ��� �����...
        CouchFine::Pool singleRepush;
        singleRepush.push_back( typelib::json::cjv( *doc.o ) );
        store << CouchFine::Mode::NewUpdate( singleRepush, doc.fnCreateJSON );

        // ...�� ������: ���� ���������� �������
            const auto& objAny = *singleRepush.front();
            const CouchFine::Object& ro = boost::any_cast< CouchFine::Object >( objAny );
            if ( CouchFine::hasError( ro ) ) {
                std::cerr << "CouchFine::operator<<( Database, NewUpdate ) " << CouchFine::error( ro ) << std::endl;
                throw CouchFine::Exception( "Unrecognized exception: " + CouchFine::error( ro ) );
            }
            // ���������� �������� �������� UID � �������
            const CouchFine::uid_t& uid = CouchFine::uid( ro );
            const CouchFine::rev_t& rev = CouchFine::revision( ro );
            CouchFine::uid( *doc.o, uid, rev );
            // @todo fine � ���� �������� ������ ����� ���������� ������? ��������
            //       � ��� �������� ������������ ������� ������� ������.

    } // else if ( doc.p )

    return store;
}





#if 0
// - @todo ...
inline CouchFine::Database& operator<<(
    CouchFine::Database& store,
    const CouchFine::Mode::AppendNewOnly& doc
) {
    throw "Not implemented.";

#if 0
// - @todo ����� �� ��� ������� map.

    const std::string& uid = doc.o.uid();
    if ( uid.empty() ) {
        // � �. �� ������ UID, ����� ������ ����� �.
        store.createDocument( doc.o );
        return store;
    }

    CouchFine::Object sDoc;
    store >> sDoc.uid( doc.o.uid() );
    if ( sDoc.empty() ) {
        // ������ �. � ��������� ��� ���, ����� ������ ����� �.
        store.createDocument( doc.o );
        return store;
    }

    // �. � �������� � 'doc' UID ���������� � ���������. ���������
    // ���������� ����������, ������� ������ *�����* ����.
    // �������� ����� �� ���� ������� ���������.
    /* - ��������.
    const auto merge = [ &sDoc ] ( const std::pair< std::string, CouchFine::Variant >&  d ) {
        const auto ftr = sDoc.find( d.first );
        if (ftr == sDoc.cend()) {
            // ����� ����, ���������
            // �� ��������� �������� �������
            if (d.first != "_rev") {
                sDoc[ d.first ] = d.second;
            }
        } else {
            // ���� ��� ����, ��, ��������, ��� ���������� �������� ��������
            const auto& type = ftr->second->type();
            if (type == typeid( CouchFine::Object )) {
                CouchFine::Object& obj =
                    boost::any_cast< CouchFine::Object >( *ftr->second );
                // ...
            }
        }
    };
    std::for_each( doc.o.cbegin(), doc.o.cend(), merge );
    */

    ����� �� ��� ������� map.

    const bool updated = (sDoc.size() != doc.o.size());
    if ( updated ) {
        store.createDocument( sDoc );
    }
#endif

    return store;
}

#endif







/**
* ������ ������ � CouchFine::Array.
*/
template< typename T >
inline CouchFine::Array& operator<<(
    CouchFine::Array& a,
    const T& v
) {
    /* - @todo fine �������� �������� �����.
    static_assert( (
        (typename( T ) == typename( double ))
     || (typename( T ) == typename( float ))
     || (typename( T ) == typename( int ))
     || (typename( T ) == typename( long ))
     || (typename( T ) == typename( size_t ))
     || (typename( T ) == typename( std::string ))
     || (typename( T ) == typename( CouchFine::Array ))
     || (typename( T ) == typename( CouchFine::Object ))
     ), "��� �� ��������������." );
    */

    a.push_back( typelib::json::cjv( v ) );
    return a;
}
