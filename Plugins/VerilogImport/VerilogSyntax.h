//-----------------------------------------------------------------------------
// File: VerilogSyntax.h
//-----------------------------------------------------------------------------
// Project: Kactus2
// Author: Esko Pekkarinen
// Date: 26.09.2014
//
// Description:
// Common elements of the Verilog language.
//-----------------------------------------------------------------------------

#ifndef VERILOGSYNTAX_H
#define VERILOGSYNTAX_H

#include <QRegularExpression>

namespace VerilogSyntax
{
    //! Used to identify the tool in environment identifiers.
    QString const TOOL_NAME = QString("kactus2.cs.tut.fi");

	//! Warning that is generated to generated files.
	const QString TAG_OVERRIDE("WARNING: EVERYTHING ON AND ABOVE THIS LINE MAY BE OVERWRITTEN BY KACTUS2!!!");

    //! Module begins with module <name> #(<parameters>) (<ports>);.
    const QRegularExpression MODULE_BEGIN("(?:^|\\r?\\n)[ \t]*(?:macro)?module\\s+(\\w+)\\s*(#\\s*[(].*[)])?\\s*(?=([(][^)]*[)])?\\s*;)?",
        QRegularExpression::DotMatchesEverythingOption);

	//! Module header begins with module <name> #(.
	const QRegularExpression MODULE_KEY_WORD("module\\s+(\\w+)\\s*(#?\\s*[(])");

    //! Module ends with keyword endmodule.
    const QRegularExpression MODULE_END("endmodule");

    //! Pattern for ranges for e.g. port sizes are given as [left:right].
    const QString RANGE("\\[.*?\\s*[:]\\s*.*?\\]");

    //! Regular expression for capturing range terms.
    const QRegularExpression CAPTURING_RANGE("\\[(.*?)\\s*[:]\\s*(.*?)\\]");

    /*  Identifiers e.g. port names may contain characters a-z, A-Z, numbers, underscores and dollar signs.
     *  Multiple identifiers declared at once must be comma separated.
     */
    const QString NAMES("[a-zA-Z0-9_$]+(?:\\s*[,]\\s*[a-zA-Z0-9_$]+)*");

    //! Pattern for one-line comments.
    const QString COMMENT("[/]{2}[ \\t]*([^\\r\\n]*)(?=\\r?\\n|$)");

    //! Single-line comments.
    const QRegularExpression COMMENTLINE("(^|\\r?\\n)[ \\t]*" + COMMENT);

    //! Multi-line comments.
    const QRegularExpression MULTILINE_COMMENT("/\\*.*?\\*/", QRegularExpression::DotMatchesEverythingOption);  

    //! An expression that may contain an operator or an alphanumeric symbol.
    const QString OPERATION_OR_ALPHANUMERIC("({[^}]*})|([$<>+*\\(\\)\\{\\}/-])|([']?[{(])|(`?\\w+)|((\\w+)?'\\w+)");
    
    //! Name + value pair, e.g. name=value.
    const QString NAME_VALUE = "\\w+\\s*=(\\s*(" + OPERATION_OR_ALPHANUMERIC + "))+";  

    /*!
     *  Returns the given name in a valid format for Verilog.
     */
    QString legalizeName(QString const& name);

    /*!
     *  Culls multi line comments and stray single line comments out of the input text.
     *
     *      @param [in] inspect   The input text to parse.
     *
     *      @return   The input without comments.
     */
    QString cullStrayComments(QString const& input);
    
    /*!
     *  Finds the position and length of the first Verilog module declaration in the input.
     *
     *      @param [in] input   The input text to parse.
     *
     *      @return   The position and length of the module declaration.
     */
    QPair<int,int> findModuleDeclaration(QString const& input);

    /*!
     *  Finds the module implementation in Verilog code.
     *
	 *      @param [in] code		            The code that will be inspected.
	 *      @param [out] implementation	        The module implementation.
     *      @param [out] postModule		        Anything that exists after the module.
     *      @param [out] error                  If fails, this will contain an error message.
	 *
	 *      @return True, if a single implementation could be found, else false.
     */
	bool selectImplementation(QString const& fileContent, QString& implementation,
        QString& postModule, QString& error);
    
    /*!
     *  Finds position of the module implementation in Verilog code.
     *
	 *      @param [in] code		            The code that will be inspected.
	 *      @param [out] implementationStart    The position where the implementation begins, if successful.
     *      @param [out] implementationEnd      The position where the implementation ends, if successful.
     *      @param [out] error                  If fails, this will contain an error message.
	 *
	 *      @return True, if a single implementation could be found, else false.
     */
    bool findImplementation(QString const& code, int& implementationStart, int& implementationEnd, QString& error);
}

#endif // VERILOGSYNTAX_H