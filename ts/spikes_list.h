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

            for(ui32 li=0; li<Info.Labels.size(); ++li) {
                const ui32& start_of_label = Info.Labels[li].From;
                const ui32& label_id = Info.Labels[li].LabelId;

                const ui32& end_of_label = Info.Labels[label_id].To;
                const string& label = Info.UniqueLabelNames[label_id];


                TSpikesList labeled_sl;
                for(ui32 di=0; di<Dim(); ++di) {
                    while( (indices[di] < Data[di].Values.size()) && (Data[di].Values[indices[di]] < end_of_label) ) {
                        if(Data[di].Values[indices[di]] >= start_of_label) {
                            labeled_sl.AddSpike(di, Data[di].Values[indices[di]] - static_cast<double>(start_of_label));
                        }
                        indices[di]++;
                    }
                }

                while(Dim() > labeled_sl.Dim()) {
                    labeled_sl.Data.emplace_back();
                }
                labeled_sl.Info.AddLabelAtPos(label, 0, end_of_label - start_of_label);
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