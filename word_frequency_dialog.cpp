#include "word_frequency_dialog.h"

#include <QAbstractItemView>
#include <QHeaderView>
#include <QTableWidgetItem>

WordFrequencyDialog::WordFrequencyDialog(QWidget* parent)
    : QDialog(parent)
{
    ui_.setupUi(this);
    ui_.frequency_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui_.frequency_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui_.frequency_table->setSelectionMode(QAbstractItemView::SingleSelection);
    ui_.frequency_table->horizontalHeader()->setStretchLastSection(true);
}

void WordFrequencyDialog::setData(const std::vector<std::pair<QString, int>>& data)
{
    ui_.frequency_table->setRowCount(static_cast<int>(data.size()));
    for (int row = 0; row < static_cast<int>(data.size()); ++row) {
        const auto& entry = data[static_cast<size_t>(row)];
        ui_.frequency_table->setItem(
            row, 0, new QTableWidgetItem(entry.first));
        ui_.frequency_table->setItem(
            row, 1, new QTableWidgetItem(QString::number(entry.second)));
    }
}
