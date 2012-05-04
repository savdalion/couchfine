#pragma once

#include "configure.h"
#include "Communication.h"


namespace CouchDB {

/**
* ��� ��������. ������������.
* ������������ ��� ���������� ��������, ��������, � ����� ���������
* �� ����� �����.
*
* @info ��-�� ������� �������������� ������� ���� (�� �������� ����
*       ����������), ����� �� ��������� �. � �����. ��������� � ���
*       � ����� ����, �������� ��� ���� �� �����������.
*/
class Pool :
    public CouchDB::Array
{
public:
    inline Pool() :
        CouchDB::Array()
    {
    }


    virtual inline ~Pool() {
    }

};


} // namespace CouchDB




inline CouchDB::Pool& operator<<(
    CouchDB::Pool& pool,
    const CouchDB::Object& doc
) {
    pool.push_back( CouchDB::cjv( doc ) );
    return pool;
}
