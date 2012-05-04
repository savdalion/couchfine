#pragma once

/**
* Include this header in your project.
*/

#include "configure.h"
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


namespace CouchDB {

/**
* ����� ���������� ������ ������ ������ � �������� CouchDb.
*
* (!) �� �������� ����������� ��� ����������: �������� ������ ������ �� ������.
*/
struct Mode {
private:
    /**
    * ������� ��������� ��� ���������� �������� � ���������.
    * ����� ��������� ������ ��� ��� ��������.
    *
    * @see Pool
    */
    struct Save {
        const Object* const o;
        const Pool* const p;

        inline Save( const Object& o ) :
            o( &o ), p( nullptr )
        {
        };

        inline Save( const Pool& p ) :
            o( nullptr ), p( &p )
        {
        };

        /* - ���������� � operator<<()
        inline Save( const T& o ) : o( o ) {};
        inline void save( Database& store ) const {
            static_assert( (
                (type == typename( Pool ))
             || (type == typename( Object ))
            ) && "Type not recognized." );
            const auto& type = typename( T );
            if (type == typename( Pool )) {
                saveMany( store );
            } else if (type == typename( Object )) {
                saveSingle( store );
            }
            throw "Type not recognized.";
        }

    private:
        inline void saveMany( Database& store ) const {
            store->createBulk( (Array)o );
        }
        inline void saveSingle( Database& store ) const {
            store.createDocument( cjv( o ) );
        }
        */
    };


    /**
    * ������� ��������� ��� ��������� �������� �� ���������.
    */
    struct Load {
        // �������� design-���������
        const std::string design;
        // �������� ������������� � �������� ������� ������� ������
        const std::string view;
        // ��������� ���� ��� ������� �� �������������
        // @see http://wiki.apache.org/couchdb/HTTP_view_API?action=show&redirect=HttpViewApi
        // @see http://wiki.apache.org/couchdb/View_collation
        const std::string key;

        // �������: ������ � ����������� ���������� ��������� � �������������� ��������
        const bool withDoc;

        // ������������ ���������� ������������ �����������. ��������
        // ������� �������� ��� ������� ���-�� ���������� � �������.
        const size_t limit;

        // ���������
        Array result;
        bool ok;
        std::shared_ptr< Exception >  exception;

        inline Load( const std::string& design, const std::string& view, const std::string& key, bool withDoc, size_t limit ) :
            design( design ), view( view ), key( key ),
            withDoc( withDoc ),
            limit( limit ),
            ok( false ), exception( nullptr )
        {
        };

    };


public:
    /**
    * ����������� ������ ����� ��������.
    * ���� UID �. ������ � �����������, ��� �. � ����� UID ����������,
    * ������������� ����������.
    * ���� UID �. �� ������, �������� ����� ��������.
    */
    struct NewOnly : public Save {
        inline NewOnly( const Object& o ) : Save( o ) {};
        inline NewOnly( const Pool&   p ) : Save( p ) {};
    };

    /**
    * ����������� ����� ��������, �� �� ����������� ������������.
    * ������� ��������� �� �����������, ������ UID.
    * ���� UID �. �� ������, �������� ����� ��������.
    */
    struct NewSkip : public Save {
        inline NewSkip( const Object& o ) : Save( o ) {};
        inline NewSkip( const Pool&   p ) : Save( p ) {};
    };

    /** - @todo �������� ������ ������������ ������� UID, ���������� ������ �����.
    * �� ��, ��� NewSkip, �� �������� �������, ����� ���������� �������������
    * ���������� �������� ��� ��������� ���-�� �����������.
    *
    struct NewSkipMany : public Save {
        inline NewSkipMany( const Object& o ) : Save( o ) {};
        inline NewSkipMany( const Pool&   p ) : Save( p ) {};
    };
    */

    /**
    * ����������� ����� �������� ��� ����������� ������������.
    * ������� ��������� �� �����������, ������ UID.
    * ���� UID �. �� ������, �������� ����� ��������.
    */
    struct NewUpdate : public Save {
        inline NewUpdate( const Object& o ) : Save( o ) {};
        inline NewUpdate( const Pool&   p ) : Save( p ) {};
    };



    /**
    * ���������� � ������������� ��� ��������� ������ ����������.
    *
    * @param withDoc ������ � ����������� ���������� ��������� �
    *        �������������� ���������.
    */
    struct View : public Load {
        inline View(
            const std::string& design,
            const std::string& view,
            const std::string& key,
            bool withDoc,
            size_t limit = 0
        ) : Load( design, view, key, withDoc, limit ) {
            assert ( !view.empty() && "�������� ������������� ������ ���� �������." );
        };
    };



#if 0
// - @todo ...
    /**
    * ���� "����-��������" ����������� � ������������� ���������.
    * ���� ����� ���������, ������ �� ��� *�� ����������������*.
    * ���� ������ ��������� ��� ��� (����������� �� UID), �������� ����� �..
    * ���� UID �. �� ������, �������� ����� �..
    */
    struct AppendNewOnly {
        const CouchDB::Object& o;
        inline AppendNewOnly( const CouchDB::Object& o ) : o( o ) {};
    };

    /**
    * ���� "����-��������" ����������� � ������������� ���������.
    * ���� ����� ���������, ������ �� ��� *�����������*.
    * ���� ������ ��������� ��� ��� (����������� �� UID), �������� ����� �..
    * ���� UID �. �� ������, �������� ����� �..
    */
    struct AppendUpdate {
        const CouchDB::Object& o;
        inline AppendUpdate( const CouchDB::Object& o ) : o( o ) {};
    };

    // @todo ...
#endif
};


} // namespace CouchDB










/**
* ������ ���������� �� ���������.
*/

inline CouchDB::Database& operator>>(
    CouchDB::Database& store,
    CouchDB::Object& doc
) {
    const std::string& uid = boost::any_cast< std::string >( *doc[ "_id" ] );
    if ( uid.empty() ) {
        // @todo extend ������ �� ����� ���������?
        throw CouchDB::Exception( "ID required." );
    }

    // �� ������
    try {
        // @todo optimize ������ �� ������� ���� ��������: ���������� parseData() 2 ����.
        const CouchDB::Document d = store.getDocument( uid );
        const auto& t = d.getData();
        doc = boost::any_cast< CouchDB::Object >( *t );

    } catch ( ... ) {
        // ������ �� �����, ������ ��������
        doc = CouchDB::Object();
    }

    return store;
}




inline CouchDB::Database& operator>>(
    CouchDB::Database& store,
    CouchDB::Mode::View& view
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
        CouchDB::Object o = store.getView( view.view, view.design, key, otherKey );
        view.result = boost::any_cast< CouchDB::Array >( *o["rows"] );

    } catch ( const CouchDB::Exception& ex ) {
        // ������ ������ � ������� �� ������
        view.result = CouchDB::Array();
        view.ok = false;
        view.exception = std::shared_ptr< CouchDB::Exception >( new CouchDB::Exception( ex ) );
    }

    return store;
}








/**
* ���������� ���������� � ���������.
*/


/**
* ��������� �������� � CouchDB.
*//* - �� ������������.
inline CouchDB::Database& operator<<(
    CouchDB::Database& store,
    const CouchDB::Variant& doc
) {
    store.createDocument( doc );
    return store;
}
*/


// ����������� ���� �����, ����� ��������� ������� �������������� �����.
template< typename T >
inline CouchDB::Database& operator<<(
    CouchDB::Database& store,
    const T& data
) {
    static_assert( (typename( T ) == typename( CouchDB::Object )),
        "����������� � CouchDB::Mode::*() ��� ������ ������ � ����������� � ���������." );
    static_assert( false, "��� �� ��������������." );
    return store;
}




inline CouchDB::Database& operator<<(
    CouchDB::Database& store,
    const CouchDB::Mode::NewOnly& doc
) {
    // 'doc' ����� ���� ����������� ��� Object ��� ��� Pool( Object )
    assert( doc.p || doc.o );

    if ( doc.p ) {
        const CouchDB::Array a = (CouchDB::Array)( *doc.p );
        store.createBulk( a );

    } else {
        store.createDocument( CouchDB::cjv( *doc.o ) );
    }

    return store;
}






inline CouchDB::Database& operator<<(
    CouchDB::Database& store,
    const CouchDB::Mode::NewSkip& doc
) {
    // @info ���������� NewUpdate, �� ����� (� �������� �������)

    // 'doc' ����� ���� ����������� ��� Object ��� ��� Pool( Object )
    assert( doc.p || doc.o );

    if ( doc.p ) {
        // �������� ������, �. � �������� ������� - ���������
        const CouchDB::Array a = (CouchDB::Array)( *doc.p );
        const CouchDB::Array r = store.createBulk( a );
        // ������ ����������: ����������� ������ ��� "������������ �������"
        // � ������������ ����������
        // @todo fine ������������� ������������ ��� ������?

    } else {
        // ���������������� ������ ���� �� - ��� �����
        CouchDB::Pool singleRepush;
        singleRepush.push_back( CouchDB::cjv( *doc.o ) );
        store << CouchDB::Mode::NewSkip( singleRepush );

    } // else if ( doc.p )

    return store;
}




inline CouchDB::Database& operator<<(
    CouchDB::Database& store,
    const CouchDB::Mode::NewUpdate& doc
) {
    // 'doc' ����� ���� ����������� ��� Object ��� ��� Pool( Object )
    assert( doc.p || doc.o );

    if ( doc.p ) {
        // �������� ������, �. � �������� ������� - �������
        const CouchDB::Array a = (CouchDB::Array)( *doc.p );
        const CouchDB::Array r = store.createBulk( a );
        // ������� ������, �������� ��������� ��� ���������� ������
        // � ���������
        CouchDB::Pool repush;
        for (auto itr = r.cbegin(); itr != r.cend(); ++itr) {
            const auto& type = itr->get()->type();
            /* - �������� ������, ��� ������.
            CouchDB::Object convertObj;
            if (type == typeid( CouchDB::Object )) {
                // @todo optimize ����� �������������� � Object. ������ �� ����������?
                convertObj = CouchDB::Object( boost::any_cast< CouchDB::Object >( **itr ) );

            } else if (type == typeid( CouchDB::Object )) {
                convertObj = boost::any_cast< CouchDB::Object >( **itr );

            } else {
                const std::string tn = type.name();
                throw CouchDB::Exception( "Unrecognized type: " + tn );
            }

            if ( !convertObj.hasError() ) {
                // �������� ������� ������� (���������� UID �.)
                continue;
            }
            */
            const CouchDB::Object& o = boost::any_cast< CouchDB::Object >( **itr );

            // ������
            if ( CouchDB::hasError( o ) ) {
                std::cerr << CouchDB::error( o ) << std::endl;
            }

            // ��������� createBulk() � �������� �����. ������� ������������ ���������
            const auto i = std::distance( r.cbegin(), itr );

            // ������� ������������ ��������
            const auto& objAny = *a.at( i );
            const CouchDB::Object& obj = boost::any_cast< CouchDB::Object >( obj );
            const std::string uid = CouchDB::uid( obj );
            CouchDB::Object sObj;
            store >> CouchDB::uid( sObj, uid );
            if ( sObj.empty() ) {
                // ������ �. � ��������� ��� ���: ��������, ������
                // ��������� ����������� ������
                throw CouchDB::Exception( "Unrecognized exception: " + CouchDB::error( obj ) );
            }

            // �. � �������� � 'doc' UID ���������� � ���������, ������� ���
            assert( !CouchDB::rev( sObj ).empty() && "������� �� ������ ���� ������." );
            repush << sObj;

        } // for (auto itr = r.cbegin(); itr != r.cend(); ++itr)

        // ��������� ��������� �������� (���������)
        if ( !repush.empty() ) {
            store << CouchDB::Mode::NewUpdate( repush );
            // @todo fine ����������� ����������� �����.
        }

    } else {
        // ���������������� ������ ���� �� - ��� �����
        CouchDB::Pool singleRepush;
        singleRepush.push_back( CouchDB::cjv( *doc.o ) );
        store << CouchDB::Mode::NewUpdate( singleRepush );

    } // else if ( doc.p )

    return store;
}





#if 0
// - @todo ...
inline CouchDB::Database& operator<<(
    CouchDB::Database& store,
    const CouchDB::Mode::AppendNewOnly& doc
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

    CouchDB::Object sDoc;
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
    const auto merge = [ &sDoc ] ( const std::pair< std::string, CouchDB::Variant >&  d ) {
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
            if (type == typeid( CouchDB::Object )) {
                CouchDB::Object& obj =
                    boost::any_cast< CouchDB::Object >( *ftr->second );
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
* ������ ������ � CouchDB::Array.
*/
template< typename T >
inline CouchDB::Array& operator<<(
    CouchDB::Array& a,
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
     || (typename( T ) == typename( CouchDB::Array ))
     || (typename( T ) == typename( CouchDB::Object ))
     ), "��� �� ��������������." );
    */

    a.push_back( CouchDB::cjv( v ) );
    return a;
}
