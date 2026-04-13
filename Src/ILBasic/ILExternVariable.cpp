#include "ILExternVariable.h"
#include "ILRef.h"
using namespace std;

ILExternVariable::ILExternVariable()
{
    this->objType = ILObject::TypeExternVariable;
}

ILExternVariable::ILExternVariable(const ILObject* parent)
{
    this->parent = const_cast<ILObject*>(parent);
    this->objType = ILObject::TypeExternVariable;
}

ILExternVariable::~ILExternVariable()
{
}


void ILExternVariable::release()
{
	for(int i = 0;i < refs.size();i++)
	{
		refs[i]->release();
        delete refs[i];
	}
    refs.clear();
    //comment = true;
	//delete this;
}
