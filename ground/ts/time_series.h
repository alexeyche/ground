#pragma once

#include "generic.h"

#include <ground/serial/proto_serial.h>
#include <ground/protos/time_series.pb.h>

namespace NGround {

    struct TTimeSeriesData : public IProtoSerial<NGroundProto::TTimeSeriesData> {
        using TElement = double;

        void SerialProcess(TProtoSerial& serial) {
            serial(Values);
        }

        TVector<double> Values;
    };


    struct TTimeSeries: public IProtoSerial<NGroundProto::TTimeSeries>, public TTimeSeriesGeneric<TTimeSeries, TTimeSeriesData> {

        void SerialProcess(TProtoSerial& serial) {
            serial(Info);
            serial(Data);
        }
    };

}

