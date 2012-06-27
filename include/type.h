#pragma once

#include "configure.h"
#include <json-type.h>
#include <string>
#include <boost/algorithm/string.hpp>
#include <boost/function.hpp>

// @todo optimize option ���������� � typelib. ��������� typelib ���� ������������.
#ifdef SAVE_ORDER_FIELDS
    #include <map>
#else
    #include <boost/unordered_map.hpp>
#endif


/**
* ��������������� ���� ������ � ������ ��� ������� ������ � ����.
*/


namespace CouchFine {

/**
* UID ���������.
*/
typedef std::string uid_t;


/**
* ������� ���������.
*/
typedef std::string rev_t;


typedef std::pair< uid_t, rev_t >  uidrev_t;



/**
* ����, ������� ��� ������ � TinyJSON.
*//* - �������� �� ���� �� typelib. ��. ����.
typedef boost::shared_ptr< boost::any >  Variant;
typedef std::deque< Variant >            Array;

#ifdef SAVE_ORDER_FIELDS
typedef std::map< std::string, Variant >  Object;
#else
typedef boost::unordered_map< std::string, Variant >  Object;
#endif
*/

typedef typelib::json::Variant  Variant;
typedef typelib::json::Array    Array;
typedef typelib::json::Object   Object;

// @see typelib::json::cjv()



/**
* ������� ��� �������� ������ JSON �� ����������� ��������.
* ����� ���� ��������������, ����� ������������� ����������� ������.
*/
typedef boost::function< std::string( const CouchFine::Variant& v ) >  fnCreateJSON_t;



// ������ ��� �������������� � Object

/**
* @return ��������������� � ��������� ���� Variant (static_cast).
*//* - @todo fine �������� static_cast � Variant. ����������� �� boost::any?
inline operator const Object& ( const Variant& var ) {
    return boost::any_cast< Object& >( *var );
}
*/



/**
* @return �������� ���� �������, ��������������� � ��������� ���� ���
*         'def' ���� ���� � �������� ������ � ������� �� �������.
*/
template < typename T >
static inline T v( const Object& o, const std::string& field, const T& def = T() ) {
    const auto ftr = o.find( field );
    return (ftr == o.cend())
        ? def : boost::any_cast< T >( *ftr->second );
}


// @alias v()
template < typename T >
static inline T value( const Object& o, const std::string& field, const T& def = T() ) {
    return v< T >( o, field, def );
}




/**
* @return UID ������� ��� ������ ������, ���� UID �� �����.
*/
static inline uid_t uid( const Object& o ) {
    auto ftr = o.find( "_id" );
    if (ftr != o.cend()) {
        return boost::any_cast< std::string >( *ftr->second );
    }
    // ��� ��������� �., UID ��� ��� �������� '_'
    ftr = o.find( "id" );
    if (ftr != o.cend()) {
        return boost::any_cast< std::string >( *ftr->second );
    }
    return "";
}


/**
* ������������� UID ��� �������.
* @return ������ �� ������, ����� ����� ���� ������ ��� ���:
*           store >> doc.uid( "MyID", "MyRevision" )
*         ����� ��� �������� �������� �� ��������� � 'doc'.
*/
static inline Object& uid( Object& o, const uid_t& u, const rev_t& r = "" ) {
    assert( !u.empty() && "������ UID." );
    assert( (o.find( "id" ) == o.cend())
        && "���������� ���� 'id'. ����������� '_id' ��� ����������� ���������� ��������� � ��������� ��� ������ ������������������." );
    o[ "_id" ] = typelib::json::cjv( u );
    if ( !r.empty() ) {
        o[ "_rev" ] = typelib::json::cjv( r );
    }
    return o;
}


static inline Object& uid( Object& o, const uidrev_t& ur ) {
    return uid( o, ur.first, ur.second );
}



/**
* @return true, ���� ����� UID.
*/
static inline bool hasUID( const Object& o ) {
    assert( (o.find( "id" ) == o.cend())
        && "���������� ���� 'id'. ��������, ������: ����������� '_id' ��� ����������� ���������� ��������� � ���������." );
    const auto ftr = o.find( "_id" );
    return (ftr != o.cend());
}



/**
* @return ������� ������� ��� ������ ������, ���� ������� �� ������.
*/
static inline rev_t revision( const Object& o ) {
    auto ftr = o.find( "_rev" );
    if (ftr != o.cend()) {
        return boost::any_cast< std::string >( *ftr->second );
    }
    // ��� ��������� �., ������� ��� ��� �������� '_'
    ftr = o.find( "rev" );
    if (ftr != o.cend()) {
        return boost::any_cast< std::string >( *ftr->second );
    }
    return "";
}



/**
* ������������� ������� ��� �������.
* @return ������ �� ������, ����� ����� ���� ������ ��� ���:
*           store >> doc.rev( "MyRevision" )
*         ����� ��� �������� �������� �� ��������� � 'doc'.
*/
static inline Object& revision( Object& o, const rev_t& r ) {
    assert( !r.empty() && "������ �������." );
    o[ "_rev" ] = typelib::json::cjv( r );
    return o;
}



/**
* @return true, ���� ������ �������.
*/
static inline bool hasRevision( const Object& o ) {
    const auto ftr = o.find( "_rev" );
    return (ftr != o.cend());
}




/**
* @return ��������� �� ������ ��� ������ ������, ���� ������ ���.
*         ������ ��� �������� ����� ��������� � �������� � ���������.
*/
static inline std::string error( const Object& o ) {
    const auto fte = o.find( "error" );
    if (fte != o.cend()) {
        std::string e = boost::any_cast< std::string >( *fte->second );
        const auto ftr = o.find( "reason" );
        if (ftr != o.cend()) {
            e += boost::any_cast< std::string >( *ftr->second );
        }
    }
    return "";
}



static inline std::string error( const Variant& var ) {
    return ( var && (var->type() == typeid( Object )) )
        ? error( boost::any_cast< Object >( *var ) )
        : "";
}





/**
* @return true, ���� ������ �������� ���������� CouchDB �� ������.
*/
static inline bool hasError( const Object& o ) {
    return (o.find( "error" ) != o.cend());
}


static inline bool hasError( const Variant& var ) {
    return var && (var->type() == typeid( Object ))
        && hasError( boost::any_cast< Object >( *var ) );
}


/**
* @return true, ���� ������ �������� ���� 'reason' (������������ ������).
*/
static inline bool hasReason( const Object& o ) {
    return (o.find( "reason" ) != o.cend());
}





/**
* @return �������� ���� 'ok' ��� false, ���� ���� �� �������. ��� ���� �����
*         ��� � ������ �������.
*/
static inline bool ok( const Object& o ) {
    const auto ftr = o.find( "ok" );
    return (ftr == o.cend())
        ? false
        : boost::any_cast< bool >( *ftr->second );
}



} // CouchFine





/**
* ���������� � ������ ���� "���� : ��������".
*//* - ���������� ��������������� ��� operator<<( Object& , uidrev_t& ).
inline CouchFine::Object& operator<<(
    CouchFine::Object& o,
    const std::pair< const std::string&, const boost::any& >&  kv
) {
    // �� ������ �������: ������������ typelib::json::cjv()
    o[ kv.first ] = typelib::json::cjv( kv.second );
    return o;
}



template< typename T >
inline CouchFine::Object& operator<<(
    CouchFine::Object& o,
    const std::pair< std::string, const T& >&  kv
) {
    o[ kv.first ] = typelib::json::cjv( kv.second );
    return o;
}
*/




inline CouchFine::Object& operator<<(
    CouchFine::Object& o,
    const CouchFine::uidrev_t& ur
) {
    CouchFine::uid( o, ur );
    return o;
}
