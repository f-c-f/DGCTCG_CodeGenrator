#include "ILExternFile.h"
#include "ILRef.h"
using namespace std;

ILExternFile::ILExternFile()
{
    this->objType = ILObject::TypeExternFile;
}

ILExternFile::ILExternFile(const ILObject* parent)
{
    this->parent = const_cast<ILObject*>(parent);
    this->objType = ILObject::TypeExternFile;
}

ILExternFile::~ILExternFile()
{
}

void ILExternFile::release()
{
    //comment = true;
	//delete this;
}
