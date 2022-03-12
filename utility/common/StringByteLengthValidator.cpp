#include "StringByteLengthValidator.h"
#include <QLineEdit>

StringByteLengthValidator::StringByteLengthValidator(int max, QString name)
{
  this->max = max;
  this->name = name;
}

QValidator::State StringByteLengthValidator::validate(QString& text, int& pos) const
{
  QValidator::State ret = State::Acceptable;
  QByteArray textArray = text.toUtf8();
  int byteSize = textArray.size();
  int maxSize = max - 1;

  if (byteSize > maxSize) {
    ret = State::Invalid;

    while (byteSize > maxSize) {
      text.remove(--pos, 1);
      textArray = text.toUtf8();
      byteSize = textArray.size();
    }
  }
  qDebug() << QString("%1[%3/%4]: %2").arg(name).arg(text).arg(byteSize).arg(maxSize);

  return ret;
}
