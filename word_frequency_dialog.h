#ifndef WORD_FREQUENCY_DIALOG_H
#define WORD_FREQUENCY_DIALOG_H

#include <QDialog>

#include "ui_word_frequency_dialog.h"

#include <utility>
#include <vector>

class WordFrequencyDialog : public QDialog {
    Q_OBJECT
public:
    explicit WordFrequencyDialog(QWidget* parent = nullptr);
    void setData(const std::vector<std::pair<QString, int>>& data);

private:
    Ui::word_frequency_dialog ui_;
};

#endif
