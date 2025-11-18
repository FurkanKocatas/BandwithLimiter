#ifndef NUMERICTABLEWIDGETITEM_H
#define NUMERICTABLEWIDGETITEM_H

#include <QTableWidgetItem>

class NumericTableWidgetItem : public QTableWidgetItem {
public:
    NumericTableWidgetItem(const QString& text, qulonglong value)
        : QTableWidgetItem(text) {
        setData(Qt::UserRole, value);
    }
    
    bool operator<(const QTableWidgetItem& other) const override {
        QVariant thisData = data(Qt::UserRole);
        QVariant otherData = other.data(Qt::UserRole);
        
        // If both have numeric data, compare numerically
        if (thisData.isValid() && otherData.isValid()) {
            qulonglong thisValue = thisData.toULongLong();
            qulonglong otherValue = otherData.toULongLong();
            return thisValue < otherValue;
        }
        
        // If only this has data, it's greater
        if (thisData.isValid() && !otherData.isValid()) {
            return false;
        }
        
        // If only other has data, it's greater
        if (!thisData.isValid() && otherData.isValid()) {
            return true;
        }
        
        // Fallback to string comparison if neither has numeric data
        return text() < other.text();
    }
};

#endif // NUMERICTABLEWIDGETITEM_H

