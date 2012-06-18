#pragma once

#include "type.h"


namespace CouchFine {

/**
* ��� ��������. ������������.
* ������������ ��� ���������� ��������, ��������, � ����� ���������
* �� ����� �����.
*
* @info ��-�� ������� �������������� ������� ���� (�� �������� ����
*       ���������� @todo ������ ������ CouchDB?), ����� �� ���������
*       �. � �����. ��������� � ��� � ����� ����, �������� ��� ����
*       �� �����������.
*/
class Pool :
    public CouchFine::Array
{
public:
    inline Pool() :
        CouchFine::Array()
    {
    }


    virtual inline ~Pool() {
    }


};


} // CouchFine





// (!) ������ Object.
inline CouchFine::Pool& operator<<(
    CouchFine::Pool& pool,
    // ��������� ������, �.�. ������ ����� �������� � ������� ���� ������.
    // ��������, _id � _rev.
    CouchFine::Variant* var
) {
    pool.push_back( *var );
    return pool;
}



// (!) ������ Object.
inline CouchFine::Pool& operator<<(
    CouchFine::Pool& pool,
    // ��������� ������, �.�. ������ ����� �������� � ������� ���� ������.
    // ��������, _id � _rev.
    CouchFine::Object* doc
) {
    pool << &CouchFine::cjv( doc );
    return pool;
}



/* - �� ���������. ������� ������� ������.
// (!) Object �� ������.
inline CouchFine::Pool& operator<<(
    CouchFine::Pool& pool,
    const CouchFine::Object& doc
) {
    pool.push_back( CouchFine::cjv( doc ) );
    return pool;
}
*/
