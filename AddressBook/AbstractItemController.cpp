#include "StdAfx.h"
#include "AbstractItemController.h"
#include "AbstractModel.h"

AbstractController::AbstractController(void)
{
}

AbstractController::~AbstractController(void)
{
}

void AbstractController::setModelData(const string &value, AbstractModel *model, int index) const
{
	// do nothing
}
