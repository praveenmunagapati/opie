
#ifndef OpieToothDevice_H
#define OpieToothDevice_H

#include <qobject.h>
#include <qstring.h>
#include <qvaluelist.h>

namespace OpieTooth {

    class Device : public QObject {
    Q_OBJECT

    public:

    /**
     * Brings up an device.
     * Usage example: new Device(/dev/ttySB0, csr)
     *
     * @param &device QString the device name
     * @param &mode QString the mode
     */
    Device(const QString &device, const QString& mode);

        /**
         * unloads the device
         */
        ~Device();

        /**
         * attach the device
         */
        void attach();

        /**
         * detach the device
         */
        void detach();

        /**
         * Is the device loaded?
         * @return bool, if the device is loaded

        bool isLoaded()const;

        /**
         * Returns the device name
         * @return QString, the device name
         */
        QString devName()const ; // hci0

    signals:

        device(const QString& device, bool up );
    };
};

#endif
