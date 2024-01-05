#pragma once

#include <QWidget>
#include "Filtering/filterwidget.h"

namespace Ui {
class ComponentBinaryExpression;
}

class ComponentBinaryExpression : public FilterWidget
{
    Q_OBJECT
    enum class Relation { LessThan, GreaterThan };

public:
    explicit ComponentBinaryExpression(QWidget *parent = nullptr);
    ~ComponentBinaryExpression();

    PointFilter::Filter get_filter() const override;

private:
    Ui::ComponentBinaryExpression *ui;
};
