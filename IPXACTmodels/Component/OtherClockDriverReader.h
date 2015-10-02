//-----------------------------------------------------------------------------
// File: OtherClockDriverReader.h
//-----------------------------------------------------------------------------
// Project: Kactus 2
// Author: Janne Virtanen
// Date: 21.09.2015
//
// Description:
// Reader class for IP-XACT OtherClockDriver element.
//-----------------------------------------------------------------------------

#ifndef OtherClockDriverReader_H
#define OtherClockDriverReader_H

#include "OtherClockDriver.h"

#include <QObject>
#include <QSharedPointer>
#include <QDomNode>
#include <IPXACTmodels/common/CommonItemsReader.h>

//-----------------------------------------------------------------------------
//! Reader class for IP-XACT OtherClockDriver element.
//-----------------------------------------------------------------------------
class IPXACTMODELS_EXPORT OtherClockDriverReader : public CommonItemsReader
{
    Q_OBJECT

public:

    /*!
     *  The constructor.
     *
     *      @param [in] parent  The owner of this reader.
     */
    OtherClockDriverReader(QObject* parent = 0);

    /*!
     *  The destructor.
     */
    ~OtherClockDriverReader();

    /*!
     *  Creates a new OtherClockDriver from a given OtherClockDriver node.
     *
     *      @param [in] OtherClockDriverNode    XML description of the OtherClockDriver.
     */
    QSharedPointer<OtherClockDriver> createOtherClockDriverFrom(QDomNode const& otherClockDriverNode) const;

	General::ClockStruct* readClockStruct(QDomNode &clockNode) const;

private:

    //! No copying allowed.
    OtherClockDriverReader(OtherClockDriverReader const& rhs);
    OtherClockDriverReader& operator=(OtherClockDriverReader const& rhs);
};

#endif // OtherClockDriverReader_H