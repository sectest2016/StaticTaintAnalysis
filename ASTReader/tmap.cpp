#include "tmap.h"

//默认构造函数，污染属性的类型对应为unknown
Tainted_Attr::Tainted_Attr(){
	u.ptrAttr = NULL;
	type = TYPE_UNKNOWN;
}

//追加了类型参数的构造函数，如果是类请传入该类的classTmap
Tainted_Attr::Tainted_Attr(eVarDeclType mytype
#ifdef USECLASS
	, classTmap *ct
#endif
	)
{
	type = mytype;
	if (mytype == TYPE_VARIABLE)
	{
		u.var.attr = UNTAINTED;
		u.var.relation = 0;
	}
	else if (mytype == TYPE_POINTER
#ifdef USECLASS
		|| mytype == TYPE_CLASSPOINTER
#endif
		)
	{
		u.ptrAttr = NULL;
	}
#ifdef USECLASS
	else if (mytype == TYPE_CLASS)
	{
		u.ptrClassDecl = new classTmap(*ct);
	}
#endif
	else
	{
		u.ptrAttr = NULL;
		type = TYPE_UNKNOWN;
	}
}

//拷贝构造函数
Tainted_Attr::Tainted_Attr(Tainted_Attr& b)
{
	type = b.type;
	switch (b.type)
	{
	case TYPE_VARIABLE:
		u.var.attr = b.u.var.attr;
		u.var.relation = b.u.var.relation;
		break;
#ifdef USECLASS
	case TYPE_CLASS:
		u.ptrClassDecl = b.u.ptrClassDecl;
		break;
#endif
	case TYPE_POINTER:
		u.ptrAttr = b.u.ptrAttr;
		break;
	case TYPE_UNKNOWN:
		break;
	}
}

//获取所存储的污染属性的类型
eVarDeclType Tainted_Attr::getType()
{
	return type;
}

//获取变量类型的污染属性的污染情况
e_tattr Tainted_Attr::getVariableAttr()
{
	return u.var.attr;
}

//获取变量类型的污染属性的关联
unsigned Tainted_Attr::getVariableRelation()
{
	return u.var.relation;
}

#ifdef USECLASS
//获取类类型的污染属性的classTmap指针
classTmap *Tainted_Attr::getClassDecl()
{
	return u.ptrClassDecl;
}
#endif

//获得指针类型的污染属性所指向的污染属性
Tainted_Attr *Tainted_Attr::getPointerAttr()
{
	return u.ptrAttr;
}

//信息输出函数 调试用
void Tainted_Attr::output()
{
	if (type == TYPE_VARIABLE)
	{
		if (u.var.attr == TAINTED)
			cout << "TAINTED ";
		else if (u.var.attr == UNTAINTED)
			cout << "UN ";
		else
			cout << "RE ";
		cout << u.var.relation;
	}
	//here to add output
	else
	{
		cout << "unknown type" << endl;
	}
}

//复制p中的污染属性
void Tainted_Attr::copy(Tainted_Attr *p)
{
#ifdef USECLASS
	if (type == TYPE_CLASS)
	{
		u.ptrClassDecl->~classTmap();
		delete u.ptrClassDecl;
	}
#endif

	type = p->type;
	if (type == TYPE_VARIABLE)
	{
		u.var.attr = p->u.var.attr;
		u.var.relation = p->u.var.relation;
	}
	else if (type == TYPE_POINTER
#ifdef USECLASS
		|| type == TYPE_CLASSPOINTER
#endif
		)
	{
		u.ptrAttr = p->u.ptrAttr;
	}

#ifdef USECLASS
	else if (type == TYPE_CLASS)
	{
		u.ptrClassDecl = new classTmap(*p->u.ptrClassDecl);
	}
#endif
}

//信息设置函数，如果当前污染属性的类型不为VARIABLE，不会进行修改，并警告
void Tainted_Attr::var_attr_set(e_tattr a, unsigned r)
{
	//cout << "relation = " << r << endl;
	if (type != TYPE_VARIABLE)
	{
		cout << "warning: type != TYPE_VARIABLE" << endl;
		return;
	}
	if (type == TYPE_VARIABLE)
	{
		u.var.attr = a;
		u.var.relation = r;
	}
}

#ifdef USECLASS
//信息设置函数，如果当前的污染属性的类型不为CLASS，不会进行修改，并警告
void Tainted_Attr::class_attr_set(e_tattr a, unsigned r, Expr *ptrExp)
{
	if (type != TYPE_CLASS)
	{
		cout << "warning: type != TYPE_CLASS" << endl;
		return;
	}
	//here to add
}
#endif

//信息设置函数，如果当前的污染属性的类型不为POINTER，不会进行修改，并警告
void Tainted_Attr::pointer_attr_set(e_tattr a, unsigned r)
{

	if (type != TYPE_POINTER)
	{
		cout << "warning: type != TYPE_POINTER" << endl;
		return;
	}
	u.ptrAttr->var_attr_set(a, r);
}

#ifdef USECLASS
//将已生成好的类成员自身的表链接到属性上
void Tainted_Attr::classmember_set(classTmap *ct)
{
	if (u.ptrClassDecl != NULL)
	{
		u.ptrClassDecl->~classTmap();
		delete u.ptrClassDecl;
	}
	u.ptrClassDecl = ct;
}
#endif

#ifdef USECLASS
//类指针的污染属性设置
void Tainted_Attr::classpointer_attr_set(e_tattr a, unsigned r, Expr *ptrExp)
{
	if (type != TYPE_CLASSPOINTER || ptrExp == NULL)
	{
		cout << "Warning: classpointer" << endl;
		return;
	}
	//here to add
}
#endif

//将当前污染属性指向pt指向的位置，如果当前污染属性的类型不为POINTER，不会进行修改，并警告
void Tainted_Attr::setPointer(Tainted_Attr *pt)
{
	if (type != TYPE_POINTER
#ifdef USECLASS
		|| type != TYPE_CLASSPOINTER
#endif
		)
	{
		cout << "Warning: type != POINTER" << endl;
		return;
	}
	while (1)
	{
		if (pt->type == TYPE_VARIABLE
#ifdef USECLASS
			|| pt->type == TYPE_CLASS
#endif
			)
		{
			u.ptrAttr = pt;
			return;
		}
		pt = pt->u.ptrAttr;
	}
}

//设置污染属性的类型
void Tainted_Attr::setType(eVarDeclType tp)
{
	type = tp;
	if (tp == TYPE_VARIABLE)
	{
		u.var.attr = UNTAINTED;
		u.var.relation = 0;
	}
	else if (tp == TYPE_POINTER)
	{
		u.ptrAttr = NULL;
	}
#ifdef USECLASS
	else if (tp == TYPE_CLASS)
	{
		u.ptrClassDecl = NULL;
	}
	else if (tp == TYPE_CLASSPOINTER)
	{
		u.ptrAttr = NULL;
	}
#endif
	else
	{
		u.ptrAttr = NULL;
		type = TYPE_UNKNOWN;
	}
}

//将两个污染属性取并，有待修改
void Tainted_Attr::AndAttr(Tainted_Attr &b)
{
	if (type != b.type)
	{
		cout << "Error in AndAttr()" << endl;
		return;
	}
	if (type == TYPE_VARIABLE)
	{
		if (u.var.attr == TAINTED)
			return;
		if (b.u.var.attr == TAINTED)
		{
			u.var.attr = TAINTED;
			return;
		}
		if (u.var.attr == UNTAINTED)
		{
			u.var.attr = b.u.var.attr;
			u.var.relation = b.u.var.relation;
			return;
		}
		if (b.u.var.attr == UNTAINTED)
			return;
		u.var.attr = b.u.var.attr;
		u.var.relation |= b.u.var.relation;
	}
}

//构造函数
CTmap::CTmap(){}

//拷贝构造函数
CTmap::CTmap(CTmap& b)
{
	Tainted_Attr *t = NULL, *newattr;
	VarDecl *pdec = NULL;
	map<VarDecl *, Tainted_Attr *>::iterator it = b.tmap.begin(), it_end = b.tmap.end();

	while (it != it_end)
	{
		pdec = (*it).first;
		t = (*it).second;
		//pdec==class to add how to copy


		newattr = new Tainted_Attr;
		newattr->setType(TYPE_VARIABLE);
		newattr->var_attr_set(t->getVariableAttr(), t->getVariableRelation());

		tmap[pdec] = newattr;
		it++;
	}
}

//析构函数
CTmap::~CTmap()
{
	Tainted_Attr *t;
#ifdef USECLASS
	classTmap *ct;
#endif
	map<VarDecl *, Tainted_Attr *>::iterator iter = tmap.begin(), iter_end = tmap.end();
	while (iter != iter_end)
	{
		t = iter->second;
#ifdef USECLASS
		if (t->getType() == TYPE_CLASS)
		{
			ct = t->getClassDecl();
			ct->clearTmap();
			delete ct;
		}
#endif
		delete iter->second;
		iter->second = NULL;
		iter++;
	}
	tmap.clear();
}

//map中的元素及对应的污染情况输出
void CTmap::output()
{
	map<VarDecl *, Tainted_Attr *>::iterator iter = tmap.begin(), iter_end = tmap.end();
	while (iter != iter_end)
	{
		std::cout << iter->first->getQualifiedNameAsString() << " ";
		iter->second->output();
		cout << endl;
		iter++;
	}
}

//将当前map清空，并将b中的元素及污染属性整个拷贝到map中
void CTmap::CopyMap(CTmap& b)
{
	clear();
	Tainted_Attr *t = NULL, *newattr;
	VarDecl *pdec = NULL;
	map<VarDecl *, Tainted_Attr *>::iterator it = b.tmap.begin(), it_end = b.tmap.end();

	while (it != it_end)
	{
		pdec = it->first;
		t = it->second;
		//pdec==class here to add how to copy

		newattr = new Tainted_Attr;

		newattr->copy(t);

		tmap[pdec] = newattr;
		it++;
	}
}

//若p不在表中，插入一个以p为索引的空条目
void CTmap::insert(VarDecl *p)
{
	Tainted_Attr *t = new Tainted_Attr();

	int count;
	count = tmap.count(p);
	if (count == 0)
	{
		tmap[p] = t;
		//==class here to add how to insert
	}
	else
		delete t;
}

//从map中删除p
void CTmap::del(VarDecl *p)
{
	Tainted_Attr *t = tmap[p];
	//==class here to add how to delete
#ifdef USECLASS
	if(t->getType() == TYPE_CLASS)
	{
		t->getClassDecl()->~classTmap();
		delete t->getClassDecl();
	}
#endif
	delete t;
	tmap.erase(p);
}

//取得变量定义节点p对应的污染属性
Tainted_Attr *CTmap::getAttr(VarDecl *p)
{
	int count;
	count = tmap.count(p);
	if (count == 0)
		return NULL;
	else
	{
		return tmap[p];
	}
}

#ifdef USECLASS
//获取类的变量的自身的map
classTmap *CTmap::getClassTmap(VarDecl *p)
{
	int count;
	count = tmap.count(p);
	if (count == 0)
	{
		return NULL;
	}
	else
	{
		if (tmap[p]->getType() != TYPE_CLASS)
		{
			return NULL;
		}
		return tmap[p]->getClassDecl();
	}
}
#endif

//设置某个变量的属性（变量、指针、类）
void CTmap::setType(VarDecl *p, eVarDeclType tp)
{
	int count;
	count = tmap.count(p);
	if (count == 0)
	{
		cout << "Error: No such variable in the function" << endl;
		return;
	}
	else
	{
		tmap[p]->setType(tp);
	}
}

//设置p的污染属性，p为普通变量类型
void CTmap::var_attr_set(VarDecl *p, e_tattr e, unsigned r)
{
	//cout << "relation = " << r << endl;
	int count;
	Tainted_Attr *tp;
	count = tmap.count(p);
	if (count == 0)
	{
		cout << "Error: No such variable in the function" << endl;
		return;
	}
	else
	{
		tp = tmap[p];
		if (tp->getType() != TYPE_VARIABLE)
		{
			cout << "Warning: type != TYPE_VARIABLE" << endl;
			return;
		}
		tp->var_attr_set(e, r);
	}
}

//设置pt指向的变量
void CTmap::ptr_set(VarDecl *p, Tainted_Attr *tp)
{
	int count;
	count = tmap.count(p);
	if (count == 0)
	{
		cout << "Error: No such variable in the function" << endl;
		return;
	}
	else
	{
		if (tmap[p]->getType() != TYPE_POINTER
#ifdef USECLASS
			|| tmap[p]->getType() != TYPE_CLASSPOINTER
#endif
			)
		{
			cout << "Warning: type != POINTER" << endl;
			return;
		}
		tmap[p]->setPointer(tp);
	}
}

//设置pt指向的变量的污染属性
void CTmap::ptr_attr_set(VarDecl *p, e_tattr e, unsigned r)
{
	int count;
	Tainted_Attr *tp;
	count = tmap.count(p);
	if (count == 0)
	{
		cout << "Error: No such variable in the function" << endl;
		return;
	}
	else
	{
		tp = tmap[p];
		if (tp->getType() != TYPE_POINTER)
		{
			cout << "Warning: type != TYPE_POINTER" << endl;
			return;
		}
		tp->pointer_attr_set(e, r);
	}
}

#ifdef USECLASS
//将classTmap的指针链接到VarDecl的条目上
void CTmap::classmember_attr_set(VarDecl *p, classTmap *ct)
{
	int count;
	Tainted_Attr *tp;
	count = tmap.count(p);
	if (count == 0)
	{
		cout << "Error: No such variable in the function" << endl;
		return;
	}
	else
	{
		tp = tmap[p];
		if (tp->getType() != TYPE_CLASS)
		{
			cout << "Warning: type != TYPE_POINTER" << endl;
			return;
		}
		else
		{
			tp->classmember_set(ct);
		}
	}
}

//设置污染属性
void CTmap::classmember_attr_set(VarDecl *p, e_tattr e, unsigned r, Expr *ptrExpr)
{}
#endif

//将两个map中的污染属性合并
void CTmap::AndMap(CTmap &b)
{
	VarDecl *p;
	map<VarDecl *, Tainted_Attr *>::iterator iter = tmap.begin(), iter_end = tmap.end();
	while (iter != iter_end)
	{
		p = (*iter).first;
		if (b.getAttr(p) != NULL)
			(*iter).second->AndAttr(*b.getAttr(p));
		iter++;
	}
}

//清空map中的元素
void CTmap::clear()
{
	Tainted_Attr *t;
#ifdef USECLASS
	classTmap *ct;
#endif
	map<VarDecl *, Tainted_Attr *>::iterator iter = tmap.begin(), iter_end = tmap.end();
	while (iter != iter_end)
	{
		t = iter->second;
#ifdef USECLASS
		if (t->getType() == TYPE_CLASS)
		{
			ct = t->getClassDecl();
			ct->clearTmap();
		}
#endif
		delete iter->second;
		iter->second = NULL;
		iter++;
	}
	tmap.clear();
}

//获取第n个变量定义
VarDecl *CTmap::get_VarDecl(int n)
{
	int i = 0;
	map<VarDecl *, Tainted_Attr *>::iterator iter = tmap.begin(), iter_end = tmap.end();
	for (; iter != iter_end; iter++)
	{
		if (i == n)
			return iter->first;
		i++;
	}
	return NULL;
}
