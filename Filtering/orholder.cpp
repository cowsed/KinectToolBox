#include "orholder.h"
#include "ui_orholder.h"

OrHolder::OrHolder(QWidget *parent)
    : FilterWidget(parent)
    , ui(new Ui::OrHolder)
{
    ui->setupUi(this);
}

PointFilter::Filter OrHolder::get_filter() const
{
    return PointFilter::Or(ui->widgetA->get_filter(), ui->widgetB->get_filter());
}

OrHolder::~OrHolder()
{
    delete ui;
}
