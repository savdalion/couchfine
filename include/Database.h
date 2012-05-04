#pragma once

#include "configure.h"
#include "Document.h"


namespace CouchDB{

class Database{
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


      std::vector<Document> listDocuments();
      Document getDocument(const std::string&, const std::string& rev = "" );


      inline bool hasDocument( const std::string& id ) {
        // @todo optimize?
        const std::string url = "/" + name + "/" + id;
        const Variant var = comm.getData( url );
        Object obj = boost::any_cast< Object >( *var );

        return (obj.find( "error" ) == obj.end());
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
        auto ftr = obj.find( "error" );
        if (ftr != obj.end()) {
            throw Exception( "View '" + viewName + "': " + boost::any_cast< std::string >( *ftr->second ) );
        }

        return obj;
    }




      inline bool hasView( const std::string& viewName, const std::string& designName = "" ) {
        // @todo optimize?
        const std::string designUID = getDesignUID( designName );
        const std::string url = "/" + name + "/" + designUID + "/_view/" + viewName + "?limit=1";
        const Variant var = comm.getData( url );
        Object obj = boost::any_cast< Object >( *var );

        return (obj.find( "error" ) == obj.end());
      }



      /**
      * @return UID, ������� ����� �������������� ������������.
      *
      * @param n �������� ���������� UID.
      */
      std::vector< std::string >  getUUIDs( size_t n ) const;


      Document createDocument( const Object&, const std::string &id = "" );
      Document createDocument( const Variant&, const std::string &id = "" );
      Document createDocument( Variant, std::vector< Attachment >, const std::string &id = "" );
      Document createDocument( const std::string& json, const std::string &id = "" );

      /**
      * ���������� � ��������� ����� ����������. �������� ����������� �������
      * ������ ���������� �� �����������.
      * 
      * @return ��������� ����������. ��������� ���������� ������ ������ ������,
      *         ��� ������ � �������� (���� ����� - ������������ ������� �.).
      */
      CouchDB::Array createBulk( const CouchDB::Array& docs );


      /**
      * ����������� ��������� � ���������� �� � ��������� ��� ������ flush().
      * �������� ����������� ������� ������ ���������� �� �����������.
      *
      * @return UID ���������, ��� ������� �� *�����* �������.
      */
      std::string createBulk( const CouchDB::Object& doc );


      /**
      * ��������� ����������� ������������� ��������� � ���������. �����������
      * �����������.
      *
      * @see createBulk()
      */
      inline void flush() {
          createBulk( acc );
          acc.clear();
      }



      /**
	  * ���� rev.empty(), � ��������� �������� �������������� ������.
	  */
      void deleteDocument( const std::string& id, const std::string& rev = "" );



      /**
      * ��������� �������������.
      */
      void addView( const std::string& name, const std::string& design, const std::string& map, const std::string& reduce = "" );




   protected:
      Communication& getCommunication();




   private:
      Communication& comm;
      std::string name;


      /**
      * ����������� ��� ������ ������ createBulk( const CouchDB::Object& ).
      */
      CouchDB::Array acc;
      std::vector< std::string >  accUID;

};

}



inline std::ostream& operator<<(std::ostream& out, const CouchDB::Database& db) {
   return out << "<Database: " << db.getName() << ">";
}
