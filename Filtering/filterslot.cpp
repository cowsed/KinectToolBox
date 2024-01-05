#include "filterslot.h"
#include "Filtering/componentbinaryexpression.h"
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
    ui->verticalLayout->removeWidget(current_child);

    if (s == "AlwaysPass") {
        std::cout << "CANGED Alywas" << std::endl;
        current_child = nullptr;

        typ = SlotType::AlwaysPass;
    } else if (s == "And") {
        std::cout << "CANGED and" << std::endl;
        safe_delete_old(current_child);
        current_child = new AndHolder(this);
        ui->verticalLayout->addWidget(current_child);
        typ = SlotType::And;
    } else if (s == "Component Test") {
        std::cout << "CANGED component" << std::endl;
        safe_delete_old(current_child);
        current_child = new ComponentBinaryExpression(this);
        ui->verticalLayout->addWidget(current_child);
        typ = SlotType::ComponentTest;
    }
    std::cout << "CANGED" << s.toStdString() << std::endl;
}

FilterSlot::~FilterSlot()
{
    delete ui;
}
