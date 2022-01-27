#ifndef HIGHLIGHTER_H
#define HIGHLIGHTER_H

#include <QSyntaxHighlighter>
#include <QTextCharFormat>
#include <QRegularExpression>

QT_BEGIN_NAMESPACE
class QTextDocument;
QT_END_NAMESPACE

class JSHighLightingRule
{
public:
    JSHighLightingRule(const QString &patternStr,
                     int n,
                     const QTextCharFormat &matchingFormat) {
        originalRuleStr = patternStr;
        pattern = QRegExp(patternStr);
        nth = n;
        format = matchingFormat;
    }
    QString originalRuleStr;
    QRegExp pattern;
    int nth;
    QTextCharFormat format;
};

class JSHighLighter : public QSyntaxHighlighter
{
    Q_OBJECT

public:
    JSHighLighter(QTextDocument *parent = nullptr);

protected:
    void highlightBlock(const QString &text) override;

private:
    QStringList keywords;
    QStringList operators;
    QStringList braces;

    QHash<QString, QTextCharFormat> basicStyles;
    void initializeRules();

    //! Highlighst multi-line strings, returns true if after processing we are still within the multi-line section.
    bool matchMultiline(const QString &text, const QRegExp &delimiter, const int inState, const QTextCharFormat &style);
    const QTextCharFormat getTextCharFormat(const QString &colorName, const QString &style = QString());

    QList<JSHighLightingRule> rules;
    QRegExp triSingleQuote;
    QRegExp triDoubleQuote;
};

#endif // HIGHLIGHTER_H
