#include "JSHighLighter.h"

JSHighLighter::JSHighLighter(QTextDocument *parent)
    : QSyntaxHighlighter(parent){

    keywords << "break" << "case" << "catch" << "continue" << "default" << "delete"
            << "do" << "else" << "finally" << "for" << "function" << "if" << "in"
            << "instanceof" << "new" << "return" << "switch" << "this" << "throw"
            << "try" << "typeof" << "var" << "void" << "while" << "with" << "yield";

    operators = QStringList() << "=" <<
        // Comparison
        "==" << "!=" << "<" << "<=" << ">" << ">=" <<
        // Arithmetic
        "\\+" << "-" << "\\*" << "/" << "%" << "\\*\\*" <<
        // In-place
        "\\+=" << "-=" << "\\*=" << "/=" << "%=" <<
        // Bitwise
        "\\^" << "\\|" << "&" << "~" << ">>" << "<<";

    braces = QStringList() << "{" << "}" << "\\(" << "\\)" << "\\[" << "\\]";

    basicStyles.insert("keyword", getTextCharFormat("darkBlue", "bold"));
    basicStyles.insert("operator", getTextCharFormat("#FF7729"));
    basicStyles.insert("brace", getTextCharFormat("darkBlue"));
    basicStyles.insert("defclass", getTextCharFormat("black", "bold"));
    basicStyles.insert("string", getTextCharFormat("#5ED363","italic"));
    basicStyles.insert("string2", getTextCharFormat("#5ED363"));
    basicStyles.insert("comment", getTextCharFormat("#666666", "italic"));
    basicStyles.insert("numbers", getTextCharFormat("darkBlue"));

    triSingleQuote.setPattern("'''");
    triDoubleQuote.setPattern("\"\"\"");

    initializeRules();
}

void JSHighLighter::initializeRules()
{
    foreach (QString currKeyword, keywords){
        rules.append(JSHighLightingRule(QString("\\b%1\\b").arg(currKeyword), 0, basicStyles.value("keyword")));
    }
    foreach (QString currOperator, operators){
        rules.append(JSHighLightingRule(QString("%1").arg(currOperator), 0, basicStyles.value("operator")));
    }
    foreach (QString currBrace, braces){
        rules.append(JSHighLightingRule(QString("%1").arg(currBrace), 0, basicStyles.value("brace")));
    }

    //  'class' followed by an identifier
    // FF: originally: r'\bclass\b\s*(\w+)'
    rules.append(JSHighLightingRule("\\bclass\\b\\s*(\\w+)", 1, basicStyles.value("defclass")));

    // Numeric literals
    rules.append(JSHighLightingRule("\\b[+-]?[0-9]+[lL]?\\b", 0, basicStyles.value("numbers"))); // r'\b[+-]?[0-9]+[lL]?\b'
    rules.append(JSHighLightingRule("\\b[+-]?0[xX][0-9A-Fa-f]+[lL]?\\b", 0, basicStyles.value("numbers"))); // r'\b[+-]?0[xX][0-9A-Fa-f]+[lL]?\b'
    rules.append(JSHighLightingRule("\\b[+-]?[0-9]+(?:\\.[0-9]+)?(?:[eE][+-]?[0-9]+)?\\b", 0, basicStyles.value("numbers"))); // r'\b[+-]?[0-9]+(?:\.[0-9]+)?(?:[eE][+-]?[0-9]+)?\b'

    // Double-quoted string, possibly containing escape sequences
    // FF: originally in python : r'"[^"\\]*(\\.[^"\\]*)*"'
    rules.append(JSHighLightingRule("\"[^\"\\\\]*(\\\\.[^\"\\\\]*)*\"", 0, basicStyles.value("string")));
    // Single-quoted string, possibly containing escape sequences
    // FF: originally in python : r"'[^'\\]*(\\.[^'\\]*)*'"
    rules.append(JSHighLightingRule("'[^'\\\\]*(\\\\.[^'\\\\]*)*'", 0, basicStyles.value("string")));

    // From '//' until a newline
    // FF: originally: r'//[^\\n]*'
    rules.append(JSHighLightingRule("//[^\\n]*", 0, basicStyles.value("comment")));
}

void JSHighLighter::highlightBlock(const QString &text){
    foreach (JSHighLightingRule currRule, rules){
        int idx = currRule.pattern.indexIn(text, 0);
        while (idx >= 0){
            // Get index of Nth match
            idx = currRule.pattern.pos(currRule.nth);
            int length = currRule.pattern.cap(currRule.nth).length();
            setFormat(idx, length, currRule.format);
            idx = currRule.pattern.indexIn(text, idx + length);
        }
    }
    setCurrentBlockState(0);

    // Do multi-line strings
    bool isInMultilne = matchMultiline(text, triSingleQuote, 1, basicStyles.value("string2"));
    if (!isInMultilne)
    isInMultilne = matchMultiline(text, triDoubleQuote, 2, basicStyles.value("string2"));
}

bool JSHighLighter::matchMultiline(const QString &text,
                                 const QRegExp &delimiter,
                                 const int inState,
                                 const QTextCharFormat &style){
    int start = -1;
    int add = -1;
    int end = -1;
    int length = 0;

    // If inside triple-single quotes, start at 0
    if (previousBlockState() == inState) {
        start = 0;
        add = 0;
    }
    // Otherwise, look for the delimiter on this line
    else {
        start = delimiter.indexIn(text);
        // Move past this match
        add = delimiter.matchedLength();
    }

    // As long as there's a delimiter match on this line...
    while (start >= 0) {
        // Look for the ending delimiter
        end = delimiter.indexIn(text, start + add);
        // Ending delimiter on this line?
        if (end >= add) {
            length = end - start + add + delimiter.matchedLength();
            setCurrentBlockState(0);
        }
        // No; multi-line string
        else {
            setCurrentBlockState(inState);
            length = text.length() - start + add;
        }
        // Apply formatting and look for next
        setFormat(start, length, style);
        start = delimiter.indexIn(text, start + length);
    }
    // Return True if still inside a multi-line string, False otherwise
    if (currentBlockState() == inState)
        return true;
    else
        return false;
}

const QTextCharFormat JSHighLighter::getTextCharFormat(const QString &colorName,
                                                     const QString &style){
    QTextCharFormat charFormat;
    QColor color(colorName);
    charFormat.setForeground(color);
    if (style.contains("bold", Qt::CaseInsensitive))
        charFormat.setFontWeight(QFont::Bold);
    if (style.contains("italic", Qt::CaseInsensitive))
        charFormat.setFontItalic(true);
    return charFormat;
}
