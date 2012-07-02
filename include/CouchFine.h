#pragma once

// @todo fine ������ using namespace �� global.
// @todo fine ������ ������ �������� CouchFine:: � ����.

// @todo fine optimize ������� ����������, ������� ����� �� �����������
//       ��� ������ inline � ����������� �� ��������� �������������.

#include "configure.h"
#include "Mode.h"
#include "Communication.h"
#include "Connection.h"
#include "View.h"
#include "Database.h"
#include "Pool.h"


namespace CouchFine {


/**
* (!) ������ design-������� ������ �������������� ������ ��������� �������.
*/


// ������ ������ � �������� �������� ����, �����:
// 1. ���� ���������.
// 2. �� ����� ���� ���������� ��� inline.


/**
* ������ ���������� �� ���������.
*/


/**
* �������� ���� ��������.
*/
CouchFine::Database& operator>>(
    CouchFine::Database& store,
    CouchFine::Object& doc
);




/**
* �������� ��������� ����������.
* 'pool' ������ ��������� UID ����������.
*
* @todo extend ����������� �������� � ���� ������ ������� ���������.
*/
CouchFine::Database& operator>>(
    CouchFine::Database& store,
    CouchFine::Mode::Doc& doc
);






CouchFine::Database& operator>>(
    CouchFine::Database& store,
    CouchFine::Mode::View& view
);








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




CouchFine::Database& operator<<(
    CouchFine::Database& store,
    CouchFine::Mode::NewOnly& doc
);






CouchFine::Database& operator<<(
    CouchFine::Database& store,
    CouchFine::Mode::NewSkip& doc
);






CouchFine::Database& operator<<(
    CouchFine::Database& store,
    CouchFine::Mode::File& file
);






CouchFine::Database& operator<<(
    CouchFine::Database& store,
    CouchFine::Mode::NewUpdate& doc
);





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


} // CouchFine
