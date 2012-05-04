#pragma once

#include "configure.h"
#include <map>


namespace CouchDB{

/**
* ������������� CouchDB.
*/
struct View {

    /**
    * �������� �������������.
    */
    std::string name;

    /**
    * �������� 'map'.
    */
    std::string map;

    /**
    * �������� 'reduce'.
    */
    std::string reduce;




public:
    inline View(
        const std::string& name,
        const std::string& map,
        const std::string& reduce = ""
    ) :
        name( name ),
        map( map ),
        reduce( reduce )
    {
        assert( !name.empty() );
        assert( !map.empty() );
    }



    inline ~View() {
    }




    inline View& operator=( View& view ) {
        this->name = view.name;
        this->map = view.map;
        this->reduce = view.reduce;
        return *this;
    }



    inline bool operator==( const View& view ) {
        return (name == view.name)
            && (map == view.map)
            && (reduce == view.reduce);
    }



    /**
    * @return View, ��������������� �� ��������� �����. ����� ������
    *         ��������� �����:
    *           - map.js (�����������)
    *           - reduce.js
    */
    static View valueOf(
        const std::string& folder,
        const std::map< std::string, std::string >&  context = std::map< std::string, std::string >()
    );

};


}
