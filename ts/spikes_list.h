#pragma once

#include "time_series_info.h"

#include <ground/protos/spikes_list.pb.h>



namespace NGround {

    class TTimeSeries;

    struct TSpikesListData : public IProtoSerial<NGroundProto::TSpikesListData> {
        void SerialProcess(TProtoSerial& serial) {
            serial(Values);
        }

        TVector<double> Values;
    };


    struct TSpikesList : public IProtoSerial<NGroundProto::TSpikesList> {
        void ChangeTimeDelta(const double &dt) {
            Info.ChangeTimeDelta(dt);
            for(auto& s: Data) {
                for(auto &t: s.Values) {
                    t = ceil(t/dt);
                }
            }
        }

        const double& GetTimeDelta() const {
            return Info.Dt;
        }

        TVector<double>& operator [](const ui32 &i) {
            return Data[i].Values;
        }

        const TVector<double>& operator [](const ui32 &i) const {
            return Data[i].Values;
        }

        const ui32 Dim() const {
            return Data.size();
        }

        TTimeSeries ConvertToBinaryTimeSeries(double dt) const;

        TTimeSeries ConvertToRateVectors(double winLength) const;

        void AddSpike(ui32 ni, double t) {
            while(ni >= Data.size()) {
                Data.emplace_back();
            }
            if((Data[ni].Values.size()>0)&&(Data[ni].Values.back()>t)) {
                throw TErrException() << "Adding spike in past to spikes list. Add sorted array of spikes\n";
            }
            Data[ni].Values.push_back(t);
        }

        TVector<TSpikesList> Chop()  {
            TVector<TSpikesList> sl_chopped;

            TVector<ui32> indices(Dim(), 0);
            for (const auto& lab: Info.Labels) {
                TSpikesList labeled_sl;
                for(ui32 di=0; di<Dim(); ++di) {
                    while( (indices[di] < Data[di].Values.size()) && (Data[di].Values[indices[di]] < lab.To) ) {
                        if(Data[di].Values[indices[di]] >= lab.From) {
                            labeled_sl.AddSpike(di, Data[di].Values[indices[di]] - static_cast<double>(lab.From));
                        }
                        indices[di]++;
                    }
                }

                while(Dim() > labeled_sl.Dim()) {
                    labeled_sl.Data.emplace_back();
                }
                labeled_sl.Info.AddLabelAtPos(Info.UniqueLabelNames.at(lab.LabelId), 0, lab.To - lab.From);
                sl_chopped.push_back(labeled_sl);
            }
            L_DEBUG << "SpikesList, Successfully chopped spike lists in " << sl_chopped.size() << " chunks";
            return sl_chopped;
        }

        void SerialProcess(TProtoSerial& serial) {
            serial(Info);
            serial(Data);
        }


        TTimeSeriesInfo Info;
        TVector<TSpikesListData> Data;
    };


} // namespace NGround