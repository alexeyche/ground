#pragma once

#include "generic.h"

#include <ground/serial/proto_serial.h>
#include <ground/protos/time_series.pb.h>

namespace NGround {

    struct TTimeSeriesComplexData : public IProtoSerial<NGroundProto::TTimeSeriesData> {
        using TElement = TComplex;

        void SerialProcess(TProtoSerial& serial) {
            serial(Values);
        }

        TVector<TComplex> Values;
    };


    struct TTimeSeriesComplex: public IProtoSerial<NGroundProto::TTimeSeriesComplex>, public TTimeSeriesGeneric<TTimeSeriesComplex, TTimeSeriesComplexData> {

        void SerialProcess(TProtoSerial& serial) {
            serial(Info);
            serial(Data);
        }
    };

} // namespace NGround
