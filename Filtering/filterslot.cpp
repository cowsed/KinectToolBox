#include "filterslot.h"
#include "Filtering/componentbinaryexpression.h"
#include "Filtering/orholder.h"
#include "andholder.h"
#include "qlabel.h"
#include "ui_filterslot.h"
#include <iostream>

FilterSlot::FilterSlot(QWidget *parent)
    : FilterWidget(parent)
    , ui(new Ui::FilterSlot)
{
    ui->setupUi(this);
    ui->verticalLayout->setAlignment(Qt::AlignTop);
    current_child = nullptr;
    ui->groupBox->hide();
}

PointFilter::Filter FilterSlot::get_filter() const
{
    if (typ == SlotType::AlwaysPass) {
        return PointFilter::AlwaysTrue();
    }

    return current_child->get_filter();
}

void FilterSlot::type_changed(QString s)
{
    auto safe_delete_old = [](QWidget *old) {
        if (old != nullptr) {
            delete old;
        }
    };
    ui->childLayout->removeWidget(current_child);

    if (s == "AlwaysPass") {
        safe_delete_old(current_child);
        typ = SlotType::AlwaysPass;
        ui->groupBox->hide();

    } else if (s == "And") {
        safe_delete_old(current_child);
        current_child = new AndHolder(this);
        ui->childLayout->addWidget(current_child);
        typ = SlotType::And;
        ui->groupBox->show();
    } else if (s == "Or") {
        safe_delete_old(current_child);
        current_child = new OrHolder(this);
        ui->childLayout->addWidget(current_child);
        typ = SlotType::Or;
        ui->groupBox->show();
    } else if (s == "Component Test") {
        safe_delete_old(current_child);
        current_child = new ComponentBinaryExpression(this);
        ui->childLayout->addWidget(current_child);
        typ = SlotType::ComponentTest;
        ui->groupBox->show();
    }
}

FilterSlot::~FilterSlot()
{
    delete ui;
}
