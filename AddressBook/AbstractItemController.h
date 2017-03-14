#pragma once
#include <string>
using namespace std;

class AbstractModel;

/************************************************************************/
/*@brief The AbstractItemController class provides handling a model data,
	and it can communicate with view.
 *@author Yuri Yang <yuri.yang@liteon.com> 2016-6-21
 *@status:Unused
/************************************************************************/
class ADDRESS_BOOK_EXPORT AbstractController
{
public:
	AbstractController(void);
	virtual ~AbstractController(void);

	virtual void setModelData(const string &value, AbstractModel *model, int index) const;
	virtual bool removeItem(int index) = 0;
	virtual bool addItem(const string &value) = 0;
};
