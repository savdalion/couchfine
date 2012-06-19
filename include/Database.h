#pragma once

#include "configure.h"
#include "Document.h"


namespace CouchFine {

class Database {
   friend class Connection;

   protected:
      Database(Communication&, const std::string&);

   public:
      Database(const Database&);

      ~Database();

      
      inline Database& operator=(Database& db) {
          name = db.getName();
          return *this;
      }


      const std::string& getName() const {
          return name;
      }


      /**
      * @return �������� ���������� �� ���� ���������.
      */
      Object about() const;



      /**
      * @return ���������� ���������� � ���� ���������, �������
      *         design-���������.
      */
      size_t countDoc() const;



      /**
      * @return ���������� design-���������� � ���� ���������.
      *
      * (!) ����������� ������ design-�., �������� ������� ����������
      * �� ����. ����� � ������ ��������.
      */
      size_t countDesign() const;



      /**
      * @return UID ��� design-���������.
      */
      inline std::string getDesignUID( const std::string& designName = "" ) const {
          return "_design/" + ( designName.empty() ? name : designName );
      }


      std::vector< Document >  listDocuments();

      Document getDocument( const std::string&, const std::string& rev = "" );


      inline bool hasDocument( const std::string& id ) {
        // @todo optimize?
        const std::string url = "/" + name + "/" + id;
        const Variant var = comm.getData( url );
        const Object obj = boost::any_cast< Object >( *var );

        return !hasError( obj );
      }


      
      /**
      * @param otherKey ������ ������ ����������� � ��������� �����.
      */
      inline Object getView(
          const std::string& viewName,
          const std::string& designName = "",
          const std::string& key = "",
          const std::string& otherKey = ""
      )  {
        // ��� ����������� ������� ���� ������� ��������������
        std::string preparedKey = key;
        boost::replace_all( preparedKey, "%", "%25" );
        boost::replace_all( preparedKey, "#", "%23" );
        /* - @todo ������ ������ ����� �����: ����� �������� ��������� ����.
        boost::replace_all( preparedKey, "&", "%26" );
        */

        //const std::string k = a ? preparedKey : ("\"" + preparedKey + "\"");
        std::string k = "";
        try {
            const double t = boost::lexical_cast< double >( preparedKey );
            k = preparedKey;
        } catch ( ... ) {
            const bool a = !preparedKey.empty() && (preparedKey[0] == '[');
            if ( a ) {
                k = preparedKey;
            } else if ( !preparedKey.empty() ) {
                k = "\"" + preparedKey + "\"";
            }
        }

        const std::string designUID = getDesignUID( designName );
        const std::string url = "/" + name + "/" + designUID + "/_view/" + viewName +
            ( k.empty() ? "" : ("?key=" + k) ) +
            ( otherKey.empty() ? "" : ( ( k.empty() ? "?" : "&") + otherKey ) );

        const Variant var = comm.getData( url );
        //const auto t = var->type().name();
        const Object obj = boost::any_cast< Object >( *var );
        if ( hasError( obj ) ) {
            throw Exception( "View '" + viewName + "': " + error( obj ) );
        }

        return obj;
    }




      inline bool hasView( const std::string& viewName, const std::string& designName = "" ) {
        // @todo optimize?
        const std::string designUID = getDesignUID( designName );
        const std::string url = "/" + name + "/" + designUID + "/_view/" + viewName + "?limit=1";
        const Variant var = comm.getData( url );
        const Object obj = boost::any_cast< Object >( *var );

        return !hasError( obj );
      }



      /**
      * @return UID, ������� ����� �������������� ������������.
      *
      * @param n �������� ���������� UID.
      */
      std::vector< std::string >  getUUIDs( size_t n ) const;


      Document createDocument( const Object&, const std::string& id = "" ) const;
      Document createDocument( const Variant&, const std::string& id = "" ) const;
      Document createDocument( Variant, const std::vector< Attachment >&, const std::string& id = "" ) const;
      Document createDocument( const std::string& json, const std::string& id = "" ) const;

      /**
      * ���������� � ��������� ����� ����������. �������� ����������� �������
      * ������ ���������� �� �����������.
      * 
      * @return ��������� ����������. ��������� ���������� ������ ������ ������,
      *         ��� ������ � �������� (���� ����� - ������������ ������� �.).
      */
      CouchFine::Array createBulk(
          const CouchFine::Array& docs,
          CouchFine::fnCreateJSON_t fnCreateJSON
      );


      /**
      * ����������� ��������� � ���������� �� � ��������� ��� ������ flush().
      * �������� ����������� ������� ������ ���������� �� �����������.
      *
      * @return UID ���������, ��� ������� �� *�����* �������.
      */
      std::string createBulk(
          const CouchFine::Object& doc,
          CouchFine::fnCreateJSON_t fnCreateJSON
      );


      /**
      * ��������� ����������� ����� ��������� � ���������. ����������� ���.
      *
      * @see createBulk()
      */
      inline void flush( CouchFine::fnCreateJSON_t fnCreateJSON ) {
          createBulk( acc, fnCreateJSON );
          acc.clear();
      }



      /**
      * ���� rev.empty(), � ��������� �������� �������������� ������.
      */
      void deleteDocument( const std::string& id, const std::string& rev = "" );



      /**
      * ��������� �������������.
      */
      void addView(
          const std::string& design,
          const std::string& name,
          const std::string& map,
          const std::string& reduce = ""
      );




   protected:
      Communication& getCommunication();




   private:
      Communication& comm;
      std::string name;


      /**
      * ����������� ��� ������ ������ createBulk( const CouchFine::Object& ).
      */
      CouchFine::Array acc;
      std::vector< std::string >  accUID;

};

}




// @todo �������� ��� ���� � ������� JSON.
inline std::ostream& operator<<(std::ostream& out, const CouchFine::Database& db) {
   return out << "Database: " << db.getName() << std::endl;
}
