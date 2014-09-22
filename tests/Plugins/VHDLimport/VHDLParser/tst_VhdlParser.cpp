//-----------------------------------------------------------------------------
// File: tst_VhdlParser.cpp
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Esko Pekkarinen
// Date: 05.09.2014
//
// Description:
// Unit test for class VhdlParser.
//-----------------------------------------------------------------------------

#include <QtTest>

#include <IPXACTmodels/component.h>
#include <IPXACTmodels/port.h>
#include <IPXACTmodels/modelparameter.h>
#include <IPXACTmodels/generaldeclarations.h>

#include <common/KactusColors.h>

#include <Plugins/VHDLimport/VhdlParser.h>

#include <wizards/ComponentWizard/VhdlImportEditor/VHDLHighlighter.h>

#include <QPlainTextEdit>
#include <QSharedPointer>


Q_DECLARE_METATYPE(QSharedPointer<Port>)
Q_DECLARE_METATYPE(QSharedPointer<ModelParameter>)

class tst_VhdlParser : public QObject
{
    Q_OBJECT

public:
    tst_VhdlParser();

private slots:
    void initTestCase();
    void init();
    void cleanup();

    //! Testcases:
    void nothingParsedFromMalformedEntity();

    void nothingParsedFromMalformedEntity_data();

    void testPortIsHighlighted();
    void testPortIsHighlighted_data();

    void testGenericIsHighlighted();
    void testGenericIsHighlighted_data();

    void testFontInsideEntityIsBlackAndOutsideEntityGray();

    void testModelParameterIsAssignedToPort();
    void testModelParameterIsAssignedToPort_data();
    void testModelParameterChangeAppliesToPort();

    void testModelParameterIsAssignedToModelParameter();
    void testModelParameterIsAssignedToModelParameter_data();

    void testPortsAndModelParametersAreNotParsedOutsideEntity();

private:

    void runParser(QString const& input);

    QPlainTextEdit displayEditor_;

    VhdlParser parser_;
    
    VHDLHighlighter* highlighter_;

    QSharedPointer<Component> importComponent_;

    QSignalSpy* createdPorts_;
    QSignalSpy* createdGenerics_;

    QSignalSpy* removedPorts_;
    QSignalSpy* removedGenerics_;
    
    void verifyNotHighlightedAfterDeclartion(const int declarationStartIndex, 
        const int declarationLength, QColor const& highlightColor) const;

    void verifyDeclarationIsHighlighted(const int declarationStartIndex, const int 
        declarationLength, QColor const& expectedHighlight) const;

    void verifyNotHighlightedBeforeDeclaration(int declarationStartIndex, QColor const& highlightColor);

    void verifySectionFontColorIs(int startIndex, int endIndex, QColor const& expectedFontColor);
};

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::tst_VhdlParser()
//-----------------------------------------------------------------------------
tst_VhdlParser::tst_VhdlParser(): displayEditor_(), parser_(), 
    highlighter_(new VHDLHighlighter(&displayEditor_, this)),
    createdPorts_(0), createdGenerics_(0), importComponent_(0)
{
    qRegisterMetaType<QSharedPointer<Port> >();
    qRegisterMetaType<QSharedPointer<ModelParameter> >();

    parser_.setHighlighter(highlighter_);
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::initTestCase()
//-----------------------------------------------------------------------------
void tst_VhdlParser::initTestCase()
{

}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::init()
//-----------------------------------------------------------------------------
void tst_VhdlParser::init()
{
    importComponent_ = QSharedPointer<Component>(new Component());
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::cleanup()
//-----------------------------------------------------------------------------
void tst_VhdlParser::cleanup()
{
    parser_.clear();
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::nothingParsedFromMalformedEntity()
//-----------------------------------------------------------------------------
void tst_VhdlParser::nothingParsedFromMalformedEntity()
{
    QFETCH(QString, fileContent);

    runParser(fileContent);

    QVERIFY(!importComponent_->hasPorts());
    QVERIFY(!importComponent_->hasModelParameters());    
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::nothingParsedFromMalformedEntity_data()
//-----------------------------------------------------------------------------
void tst_VhdlParser::nothingParsedFromMalformedEntity_data()
{
    QTest::addColumn<QString>("fileContent");

    QTest::newRow("empty file") << "";

    QTest::newRow("no entity defined") << 
        "architecture rtl of empty is\n"
        "begin\n"
        "end;";

    QTest::newRow("no begin of ports") << 
        "entity test is"
        "   clk: in std_logic\n"
        ");\n"
        "end test;";

    QTest::newRow("no closing bracket for ports") << 
        "entity noPortBracket is"
        "port (\n"
        "   clk: in std_logic\n"
        "-- no closing bracket after ports.\n"
        "end noPortBracket;";

    QTest::newRow("no begin of generics") << 
        "entity test is"
        "   local_memory_addr_bits  : integer\n"
        ");\n"
        "end test;";

    QTest::newRow("no closing bracket for generics") << 
        "entity noGenericBracket is"
        "generic (\n"
        "   local_memory_addr_bits  : integer\n"
        "-- no closing bracket after generics.\n"
        "end noGenericBracket;";

    QTest::newRow("no end for entity") << 
        "entity noBracket is"
        "generic (\n"
        "   local_memory_addr_bits  : integer\n"
        ");\n"
        "port (\n"
        "   clk: in std_logic\n"
        ");";

    QTest::newRow("multiple entities") << 
        "entity firstEntity is\n"
        "generic (\n"
        "   local_memory_addr_bits  : integer\n"
        ");\n"
        "port (\n"
        "   clk: in std_logic\n"
        ");\n"
        "end firstEntity;\n"
        "\n"
        "entity secondEntity is\n"
        "generic (\n"
        "   local_memory_addr_bits  : integer\n"
        ");\n"
        "port (\n"
        "   clk: in std_logic\n"
        ");\n"
        "end secondEntity;\n";
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::runParser()
//-----------------------------------------------------------------------------
void tst_VhdlParser::runParser(QString const& input)
{
    displayEditor_.setPlainText(input);
    parser_.runParser(input, importComponent_);
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testPortHighlight()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testPortIsHighlighted()
{
    QFETCH(QString, fileContent);
    QFETCH(int, declarationStartIndex);
    QFETCH(int, declarationLength);

    runParser(fileContent);

    QColor selectedColor = KactusColors::SW_COMPONENT;
    verifyNotHighlightedBeforeDeclaration(declarationStartIndex, selectedColor);
    verifyDeclarationIsHighlighted(declarationStartIndex, declarationLength, selectedColor);
    verifyNotHighlightedAfterDeclartion(declarationStartIndex, declarationLength, selectedColor);
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testPortHighlight()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testPortIsHighlighted_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<int>("declarationStartIndex");
    QTest::addColumn<int>("declarationLength");
    
    QTest::newRow("simplest possible port declaration") <<
        "entity test is\n"
        "    port (\n"
        "        clk : in std_logic\n"
        "   );\n"
        "end test;"
        << 34 << 18;

    QTest::newRow("port declaration with default value") <<
        "entity test is\n"
        "    port (\n"
        "        clk : in std_logic := '1'\n"
        "   );\n"
        "end test;"
        << 34 << 25;

    QTest::newRow("port declaration with description") <<
        "entity test is\n"
        "    port (\n"
        "        clk : in std_logic -- Clk from top\n"
        "   );\n"
        "end test;"
        << 34 << 34;

    QTest::newRow("port declaration followed by another port") <<
        "entity test is\n"
        "    port (\n"
        "        clk : in std_logic; -- Clk from top\n"
        "        enable : in std_logic"
        "   );\n"
        "end test;"
        << 34 << 35;

    QTest::newRow("port declaration with multiple ports") <<
        "entity test is\n"
        "    port (\n"
        "        clk, enable : in std_logic -- One bit ports\n"
        "   );\n"
        "end test;"
        << 34 << 43;

    QTest::newRow("name type, direction and default value on separate lines") << 
        "entity test is\n"
        "port (clk\n"
        ":\n"
        "in\n"
        "std_logic := '1' -- Clk from top.\n"
        ");\n"
        "end test;"
        << 21 << 42;
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::verifyNotHighlightedBeforeDeclaration()
//-----------------------------------------------------------------------------
void tst_VhdlParser::verifyNotHighlightedBeforeDeclaration(int declarationStartIndex, QColor const& highlightColor)
{
    QTextCursor cursor = displayEditor_.textCursor();
    cursor.setPosition(declarationStartIndex);

    QVERIFY2(cursor.charFormat().background().color() != highlightColor, 
        "Highlight for imported declaration should not begin before declaration.");
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::verifyDeclarationIsHighlighed()
//-----------------------------------------------------------------------------
void tst_VhdlParser::verifyDeclarationIsHighlighted(const int declarationStartIndex, 
    const int declarationLength, QColor const& expectedHighlight) const
{
    QTextCursor cursor = displayEditor_.textCursor();

    for (int i = 1; i <= declarationLength; i++)
    {
        cursor.setPosition(declarationStartIndex + i);
        QVERIFY2(cursor.charFormat().background().color() == expectedHighlight,
            "Highlight is not applied to declaration.");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::verifyNotHighlightedAfterDeclartion()
//-----------------------------------------------------------------------------
void tst_VhdlParser::verifyNotHighlightedAfterDeclartion(const int declarationStartIndex, 
    const int declarationLength, QColor const& highlightColor) const
{
    QTextCursor cursor = displayEditor_.textCursor();

    cursor.setPosition(declarationStartIndex + declarationLength + 1);
    QVERIFY2(cursor.charFormat().background().color() != highlightColor,
        "Highlight for imported declaration should end after declaration.");
}


//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testGenericIsHighlighted()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testGenericIsHighlighted()
{
    QFETCH(QString, fileContent);
    QFETCH(int, declarationStartIndex);
    QFETCH(int, declarationLength);

    runParser(fileContent);

    QColor selectedColor = KactusColors::HW_BUS_COMPONENT;
    verifyNotHighlightedBeforeDeclaration(declarationStartIndex, selectedColor);
    verifyDeclarationIsHighlighted(declarationStartIndex, declarationLength, selectedColor);
    verifyNotHighlightedAfterDeclartion(declarationStartIndex, declarationLength, selectedColor);
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testGenericIsHighlighted_data()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testGenericIsHighlighted_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<int>("declarationStartIndex");
    QTest::addColumn<int>("declarationLength");

    QTest::newRow("simplest possible generic declaration") <<
        "entity test is\n"
        "    generic (\n"
        "        freq : integer\n"
        "   );"
        "end test;"
        << 37 << 14;

    QTest::newRow("generic declaration with default value") <<
        "entity test is\n"
        "    generic (\n"
        "        dataWidth_g : integer := 16);"
        "end test;"
        << 37 << 27;

    QTest::newRow("generic declaration with description") <<
        "entity test is\n"
        "    generic (\n"
        "        outputFile : string := \"target.out\" -- Some file name\n"
        "   );"
        "end test;"
        << 37 << 53;

    QTest::newRow("generic declaration followed by another generic") <<
        "entity test is\n"
        "    generic (\n"
        "        local_memory_start_addr : integer := 16#1000#;\n"
        "        local_memory_addr_bits  : integer := 12\n"
        "   );\n"
        "end test;"
        << 37 << 45;

    QTest::newRow("generic declaration with multiple generic") <<
        "entity test is\n"
        "    generic (\n"
        "        freq, dataWidth : integer --Configuration\n"
        "   );\n"
        "end test;"
        << 37 << 41;

    QTest::newRow("name type and default value on separate lines") << 
        "entity test is\n"
        "generic (local_memory_start_addr\n" 
        ": integer \n"
        ":= 16#1000#\n"
        ")\n"
        ";\n"
        "end test;"
        << 24 << 46;
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testFontInsideEntityIsBlackAndOutsideEntityGray()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testFontInsideEntityIsBlackAndOutsideEntityGray()
{
    QString fileContent = 
        "library IEEE;\n"
        "entity test is\n"        
        "end test;\n"
        "begin rtl of test\n";

    runParser(fileContent);

    verifySectionFontColorIs(1, 13, QColor("gray"));

    verifySectionFontColorIs(15, 39, QColor("black"));

    verifySectionFontColorIs(40, 58, QColor("gray"));
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::verifySectionFontColorIs()
//-----------------------------------------------------------------------------
void tst_VhdlParser::verifySectionFontColorIs(int startIndex, int endIndex, QColor const& expectedFontColor)
{
    QTextCursor cursor = displayEditor_.textCursor();

    int sectionLength = endIndex - startIndex;
    for (int i = 0; i < sectionLength; i++)
    {
        cursor.setPosition(startIndex + i);
        QVERIFY2(cursor.charFormat().foreground().color() == expectedFontColor,
            "Highlight is not applied to declaration.");
    }
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testModelParameterIsAssignedToPort()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testModelParameterIsAssignedToPort()
{
    QFETCH(QString, fileContent);
    QFETCH(int, expectedPortSize);

    runParser(fileContent);

    QCOMPARE(importComponent_->getPorts().count(), 1);
    QSharedPointer<Port> createdPort = importComponent_->getPorts().first();

    QCOMPARE(createdPort->getPortSize(), expectedPortSize);
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testModelParameterIsAssignedToPort_data()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testModelParameterIsAssignedToPort_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<int>("expectedPortSize");

    QTest::newRow("Parametrized port width.") <<
        "entity test is\n"
        "   port (\n"
        "       data : out std_logic_vector(dataWidth_g downto 0)\n"
        "   );\n"
        "   generic (\n"
        "       dataWidth_g : integer := 31\n"
        "   );\n"
        "end test;"
        << 32 ;

    QTest::newRow("Parametrized port width with subtraction.") <<
        "entity test is\n"
        "   port (\n"
        "       data : out std_logic_vector(dataWidth_g-1 downto 0)\n"
        "   );\n"
        "   generic (\n"
        "       dataWidth_g : integer := 16\n"
        "   );\n"
        "end test;"
        << 16 ;

    QTest::newRow("Parametrized port width with equation of generics.") <<
        "entity test is\n"
        "   port (\n"
        "       data : out std_logic_vector(dataWidth_g+addrWidth_g-1 downto 0)\n"
        "   );\n"
        "   generic (\n"        
        "       addrWidth_g : integer := 8;\n"
        "       dataWidth_g : integer := 16\n"
        "   );\n"
        "end test;"
        << 24 ;

    QTest::newRow("Parametrized port width with power of two.") <<
        "entity test is\n"
        "   port (\n"
        "       data : out std_logic_vector(max_g**2-1 downto 0)\n"
        "   );\n"
        "   generic (\n"
        "       max_g : integer := 8\n"
        "   );\n"
        "end test;"
        << 64 ;
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testModelParameterChangeAppliesToPort()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testModelParameterChangeAppliesToPort()
{
    QString fileContent = "entity test is\n"
        "   port (\n"
        "       data : out std_logic_vector(dataWidth_g-1 downto 0)\n"
        "   );\n"
        "   generic (\n"        
        "       dataWidth_g : integer := 8\n"
        "   );\n"
        "end test;";

    runParser(fileContent);

    QSharedPointer<Port> createdPort = importComponent_->getPorts().first();

    QCOMPARE(createdPort->getPortSize(), 8);

    QSharedPointer<ModelParameter> createdGeneric = 
        importComponent_->getModelParameters().first();
    createdGeneric->setValue("16");

    parser_.onModelParameterChanged(createdGeneric);

    QCOMPARE(createdPort->getPortSize(), 16);
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testModelParameterIsAssignedToModelParameter()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testModelParameterIsAssignedToModelParameter()
{
    QFETCH(QString, fileContent);
    QFETCH(QString, expectedValue);

    runParser(fileContent);

    QVERIFY2(!importComponent_->getModelParameters().isEmpty(), "Did not create a valid model parameter.");

    QSharedPointer<ModelParameter> createdGeneric = importComponent_->getModelParameters().last();
    QCOMPARE(createdGeneric->getValue(), expectedValue);
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testModelParameterIsAssignedToModelParameter_data()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testModelParameterIsAssignedToModelParameter_data()
{
    QTest::addColumn<QString>("fileContent");
    QTest::addColumn<QString>("expectedValue");

    QTest::newRow("Parametrized generic with other generic.") <<
        "entity test is\n"
        "   generic (\n"
        "       width_g : integer := 8;\n"
        "       dataWidth_g : integer := width_g\n"        
        "   );\n"
        "end test;"
        << "width_g" ;

    QTest::newRow("Parametrized generic with subtraction.") <<
        "entity test is\n"
        "   generic (\n"
        "       width_g : integer := 8;\n"
        "       dataWidth_g : integer := width_g-2\n"        
        "   );\n"
        "end test;"
        << "width_g-2" ;

    QTest::newRow("Parametrized generic with equation of generics.") <<
        "entity test is\n"
        "   generic (\n"
        "       base_width_g : integer := 8;\n"
        "       addrWidth_g : integer := 7;\n"
        "       dataWidth_g : integer := base_width_g+addrWidth_g+1\n"        
        "   );\n"
        "end test;"
        << "base_width_g+addrWidth_g+1" ;
}

//-----------------------------------------------------------------------------
// Function: tst_VhdlParser::testPortsAndModelParametersAreNotParsedOutsideEntity()
//-----------------------------------------------------------------------------
void tst_VhdlParser::testPortsAndModelParametersAreNotParsedOutsideEntity()
{
    QString fileContent(
        "entity empty is\n"
        "end empty;\n"
        "\n"
        "architecture structural of empty is\n"
        "\n"
        "component dut"
        "   generic (\n"        
        "       local_memory_addr_bits  : integer := 12\n"
        "   );\n"
        "   port (\n"
        "       clk : in std_logic\n"
        "   );\n"
        "end dut;\n"
        "\n"
        "end structural;\n");

    runParser(fileContent);

    QCOMPARE(importComponent_->getPorts().count(), 0);
    QCOMPARE(importComponent_->getModelParameters().count(), 0);
}

QTEST_MAIN(tst_VhdlParser)

#include "tst_VhdlParser.moc"