#pragma once
#include "Filtering/filterwidget.h"
#include "filtering.h"

#include <QWidget>

namespace Ui {
class FilterSlot;
}

class FilterSlot : public FilterWidget
{
    Q_OBJECT

    enum class SlotType { AlwaysPass, And, Or, Not, ComponentTest };

public:
    explicit FilterSlot(QWidget *parent = nullptr);
    ~FilterSlot();
    PointFilter::Filter get_filter() const override;
public slots:
    void type_changed(QString s);

private:
    Ui::FilterSlot *ui;
    SlotType typ = SlotType::AlwaysPass;
    FilterWidget *current_child;
};
