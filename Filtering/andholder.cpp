#include "andholder.h"
#include "ui_andholder.h"

AndHolder::AndHolder(QWidget *parent)
    : FilterWidget(parent)
    , ui(new Ui::AndHolder)
{
    ui->setupUi(this);
}

PointFilter::Filter AndHolder::get_filter() const
{
    return PointFilter::And(ui->widgetA->get_filter(), ui->widgetB->get_filter());
}

AndHolder::~AndHolder()
{
    delete ui;
}
