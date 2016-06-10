#pragma once

#include <ground/base/base.h>
#include <ground/pretty_print.h>
#include <ground/protos/time_series_info.pb.h>
#include <ground/serial/proto_serial.h>
#include <ground/maybe.h>

namespace NGround {

    struct TLabel: public IProtoSerial<NGroundProto::TLabel> {
        TLabel() {}

        TLabel(ui32 labelId, ui32 from, ui32 to)
            : LabelId(labelId)
            , From(from)
            , To(to)
        {}

        void SerialProcess(TProtoSerial& serial) override {
            serial(LabelId);
            serial(From);
            serial(To);
        }

        bool operator == (const TLabel& other) const {
            return LabelId == other.LabelId;
        }

        ui32 LabelId;
        ui32 From;
        ui32 To;
    };

    struct TTimeSeriesInfo: public IProtoSerial<NGroundProto::TTimeSeriesInfo> {

        void SerialProcess(TProtoSerial& serial) {
            serial(Labels);
            serial(UniqueLabelNames);
            serial(Dt);
            serial(DimSize);
        }

        void AddLabelAtPos(const string &lab, ui32 pos, ui32 dur) {
            if (Labels.size() > 0) {
                if (Labels.front().To > pos) {
                    throw TErrException() << "Trying to add non increasing label: " << Labels.front().To << " >= " << pos;
                }
            }

            Labels.emplace_back();
            Labels.back().From = pos;
            Labels.back().To = pos + dur;

            Labels.back().LabelId = std::distance(UniqueLabelNames.begin(), std::find(UniqueLabelNames.begin(), UniqueLabelNames.end(), lab));
            if (Labels.back().LabelId == UniqueLabelNames.size()) {
                UniqueLabelNames.push_back(lab);
            }
        }

        void ChangeTimeDelta(double dt) {
            Dt = dt;
            for(auto& lab: Labels) {
                lab.From = ceil(lab.From/dt);
                lab.To = ceil(lab.To/dt);
            }
        }

        bool operator == (const TTimeSeriesInfo &l) const {
            if(Labels != Labels) return false;
            return true;
        }
        bool operator != (const TTimeSeriesInfo &l) const {
            return ! (*this == l);
        }

        TMaybe<ui32> GetClassId(double t) const {
            while(__CurrentPosition < Labels.size()) {
                const auto& label = Labels.at(__CurrentPosition);
                if (label.To < t) {
                    if (label.From >= t) {
                        return label.LabelId;
                    }
                } else {
                    break;
                }
                __CurrentPosition += 1;
            }
            return Nothing<ui32>();
        }

        double GetDuration() const {
            if (Labels.size() == 0) {
                return 0.0;
            }
            return Labels.back().To;
        }

        TVector<TLabel> Labels;
        TVector<TString> UniqueLabelNames;

        double Dt = 1.0;
        ui32 DimSize = 0;

        mutable ui32 __CurrentPosition = 0;
    };


}