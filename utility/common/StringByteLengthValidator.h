#pragma once

#include <QValidator>

class StringByteLengthValidator : public QValidator
{
private:
  QString name;
  int max;

public:
  StringByteLengthValidator(int max, QString name = "Text");

  // Inherited via QValidator
  virtual QValidator::State validate(QString& text, int& pos) const override;
};

