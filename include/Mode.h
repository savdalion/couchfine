#pragma once

#include "configure.h"
#include "type.h"
#include "Pool.h"
#include "Exception.h"
#include <vector>


namespace CouchFine {

/**
* ����� ���������� ������ ������ � �������� CouchDB.
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
        Object* const o;
        Pool* const p;
        const fnCreateJSON_t fnCreateJSON;

        inline Save( Object& o, fnCreateJSON_t fnCreateJSON ) :
            o( &o ), p( nullptr ), fnCreateJSON( fnCreateJSON )
        {
        };

        inline Save( Pool& p, fnCreateJSON_t fnCreateJSON ) :
            o( nullptr ), p( &p ), fnCreateJSON( fnCreateJSON )
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
            store.createDocument( typelib::json::cjv( o ) );
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

        /**
        * ��� ������� �� ������������� ������������ ������ ����
        * "����1=��������1&����2=��������2&...".
        * �������:
        *   (1) startkey="_design/a"&endkey="_design/{"
        *   (2) key=1000
        *   (3) descending=true&stale=ok
        *
        * @see http://wiki.apache.org/couchdb/HTTP_view_API
        * @see http://wiki.apache.org/couchdb/View_collation
        *
        * (!) ���� 'keys' ����� �������������� ������ � POST-��������.
        * @see http://wiki.apache.org/couchdb/HTTP_view_API#Querying_Options
        */
        const std::string key;

        // �������: ������ � ����������� ���������� ��������� � �������������� ��������
        const bool withDoc;

        // ������������ ���������� ������������ �����������. ��������
        // ������� �������� ��� ������� ���-�� ���������� � �������.
        const size_t limit;

        // ���������
        // (!) ��� ������������� ������ limit / offset, �������� 'totalRows'
        // �� ��������� � 'result.count()'; 'totalRows' - ��� ���-��, �������,
        // ������� *�����* ���������� �������������.
        size_t totalRows;
        Array result;
        bool ok;
        std::shared_ptr< Exception >  exception;


        inline Load(
            const std::string& design,
            const std::string& view,
            const std::string& key,
            bool withDoc,
            size_t limit
        ) :
            design( design ), view( view ), key( key ),
            withDoc( withDoc ),
            limit( limit ),
            ok( false ), exception( nullptr )
        {
#ifdef _DEBUG
            if ( !key.empty() ) {
                assert( ( (key[0] != '?') && (key[0] != '&') )
                    && "������ ������ �� ������ ���������� �� '?' ��� '&'." );
                assert( (key.find( '=' ) != std::string::npos)
                    && "������ ������ ������ ��������� ������ ���� '����1=��������1&����2=��������2&...'." );
            }
#endif
        };


        inline Load( size_t limit ) :
            design(), view(), key(),
            withDoc( true ),
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
        inline NewOnly( Object& o, fnCreateJSON_t fnCreateJSON ) : Save( o, fnCreateJSON ) {};
        inline NewOnly( Pool& p, fnCreateJSON_t fnCreateJSON ) : Save( p, fnCreateJSON ) {};
    };

    /**
    * ����������� ����� ��������, �� �� ����������� ������������.
    * ������� ��������� �� �����������, ������ UID.
    * ���� UID �. �� ������, �������� ����� ��������.
    */
    struct NewSkip : public Save {
        inline NewSkip( Object& o, fnCreateJSON_t fnCreateJSON ) : Save( o, fnCreateJSON ) {};
        inline NewSkip( Pool& p, fnCreateJSON_t fnCreateJSON ) : Save( p, fnCreateJSON ) {};
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
    * ���������� �������� �������� UID � REV ��� ������������ ��������
    * ������� � ����.
    */
    struct NewUpdate : public Save {
        inline NewUpdate( Object& o, fnCreateJSON_t fnCreateJSON ) : Save( o, fnCreateJSON ) {};
        inline NewUpdate( Pool& p, fnCreateJSON_t fnCreateJSON ) : Save( p, fnCreateJSON ) {};
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




    /**
    * ���������� � ��������� ��� ��������� ������ ����������.
    *
    * @param  ������ � ����������� ���������� ��������� �
    *        �������������� ���������.
    */
    struct Doc : public Load {
        /**
        * ������ UID ��� ������� ����������.
        */
        const std::vector< typelib::uid_t >&  uid;

        inline Doc(
            const std::vector< typelib::uid_t >&  uid,
            size_t limit = 0
        ) :
            Load( limit ),
            uid( uid )
        {
            assert ( !uid.empty() && "�������� ������������� ������ ���� �������." );
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
        const CouchFine::Object& o;
        inline AppendNewOnly( const CouchFine::Object& o ) : o( o ) {};
    };

    /**
    * ���� "����-��������" ����������� � ������������� ���������.
    * ���� ����� ���������, ������ �� ��� *�����������*.
    * ���� ������ ��������� ��� ��� (����������� �� UID), �������� ����� �..
    * ���� UID �. �� ������, �������� ����� �..
    */
    struct AppendUpdate {
        const CouchFine::Object& o;
        inline AppendUpdate( const CouchFine::Object& o ) : o( o ) {};
    };

    // @todo ...
#endif




    /**
    * ��������� ��� ���������� � ��������� ���������� � ���� �����.
    */
    struct File {
        /**
        * �������� � ���������� �����.
        */
        const std::string name;
        const std::string data;

        /**
        * UID � ������� ���������, � �������� ����������� ����.
        */
        const uid_t uidDoc;
        const rev_t revisionDoc;

        /**
        * ���� ������
        */

        inline File(
            const std::string& name, const std::string& data,
            const uid_t& uidDoc, const rev_t& revisionDoc
        ) :
            name( name ), data( data ),
            uidDoc( uidDoc ), revisionDoc( revisionDoc )
        {
        };

        /**
        * �������, �� �������� ������������ ������������� ����������� ����
        * ������ ���� ���������.
        */
        static std::string const& PREFIX() {
            const static std::string r = "file://";
            return r;
        }

    };



};


} // namespace CouchFine
