#include "spikes_list.h"

#include "time_series.h"

namespace NGround {

    TTimeSeries TSpikesList::ConvertToBinaryTimeSeries(double dt) const {
        TTimeSeries out;

        out.Info = Info;
        out.SetDimSize(Dim());
        double max_spike_time = std::numeric_limits<double>::min();
        ui32 max_size = std::numeric_limits<ui32>::min();
        for (ui32 di=0; di<Data.size(); ++di) {
            double t=0;
            for (const auto& spike_time: Data[di].Values) {
                while (t < spike_time) {
                    out.AddValue(di, 0.0);
                    t += dt;

                }
                out.AddValue(di, 1.0);
                t += dt;
                if (spike_time > max_spike_time) {
                    max_spike_time = spike_time;
                }
            }
            max_size = std::max(max_size, out.Data[di].Values.size());
        }

        for(ui32 di=0; di<out.Data.size(); ++di) {
            while (out.Data[di].Values.size() < max_size) {
                out.AddValue(di, 0.0);
            }
        }

        return out;
    }

    // TTimeSeries TSpikesList::ConvertToRateVectors(double winLength) const {
    //     TTimeSeries out;

    //     out.SetDimSize(Dim());

    //     double minSpikeTime = std::numeric_limits<double>::max();

    //     for (ui32 di=0; di<Data.size(); ++di) {
    //         if (Data[di].Values.size() > 0) {
    //             minSpikeTime = std::min(minSpikeTime, Data[di].Values.front());
    //         }
    //     }

    //     TVector<ui32> listIndices(Dim(), 0);

    //     TDeque<TPair<double, double>> silenceDiff;
    //     silenceDiff.emplace_back(0.0, minSpikeTime);

    //     bool weAreDone = false;
    //     while (!weAreDone) {
    //         double minNextSpikeTime = std::numeric_limits<double>::max();
    //         TVector<ui32> window(Dim(), 0);

    //         for (ui32 di=0; di<Data.size(); ++di) {
    //             while (listIndices[di] < Data[di].Values.size()) {
    //                 const double& newSpike = Data[di].Values[ listIndices[di] ];
    //                 if (newSpike <= (minSpikeTime + winLength)) { // in a window
    //                     window[di] += 1;
    //                 } else {
    //                     minNextSpikeTime = std::min(minNextSpikeTime, newSpike);
    //                     break;
    //                 }
    //                 ++listIndices[di];
    //             }
    //         }
    //         for (ui32 di=0; di<Data.size(); ++di) {
    //             out.AddValue(di, static_cast<double>(window[di])/winLength);
    //         }

    //         if (minNextSpikeTime == std::numeric_limits<double>::max()) {
    //             weAreDone = true;
    //         } else {
    //             silenceDiff.emplace_back(minSpikeTime + winLength, minNextSpikeTime);
    //             minSpikeTime = minNextSpikeTime;
    //         }
    //     }

    //     double cumulativeShift = 0.0;
    //     ui32 prevLabelTo = 0;
    //     for (const auto& lt: Info.Labels) {
    //         L_INFO << "Considering label (" << lt.From << ", " << lt.To << ")";

    //         double labelFrom = lt.From ;
    //         double labelTo = lt.To;

    //         double labelCumulativeDiff = 0.0;
    //         while ((silenceDiff.size() > 0) && (silenceDiff.front().second < lt.To)) {
    //             const auto& sd = silenceDiff.front(); 
    //             double diff = sd.second  - sd.first;
    //             if (sd.first < labelFrom) {
    //                 labelFrom = sd.first;
    //             }    
    //             silenceDiff.pop_front();
    //         }
            
    //         labelFrom -= cumulativeShift;
    //         labelTo -= cumulativeShift;
            
    //         L_INFO << "Result cum diff " << cumulativeShift;
            
    //         L_INFO << "Result label: (" << labelFrom << ", " << labelTo << ")";

    //         labelFrom = std::max(prevLabelTo, static_cast<ui32>(floor(labelFrom/winLength)));
    //         labelTo = ceil(labelTo/winLength);

    //         prevLabelTo = labelTo;

    //         L_INFO << "Result label after ceil: (" << labelFrom << ", " << labelTo << ")";
    //         out.Info.AddLabelAtPos(Info.UniqueLabelNames.at(lt.LabelId), labelFrom, labelTo-labelFrom);

    //         // cumulativeShift += labelCumulativeDiff;
    //     }
    //     return out;
    // }

    TTimeSeries TSpikesList::ConvertToRateVectors(double winLength) const {
        TTimeSeries out;
        out.SetDimSize(Dim());    
        
        TVector<ui32> listIndices(Dim(), 0);
        double winStart = 0.0;

        double timeEnd = -1;
        if (Info.Labels.size() > 0) {
            timeEnd = Info.Labels.back().To;
        }

        bool weAreDone = false;
        while (!weAreDone) {
            weAreDone = true;
            
            TVector<ui32> window(Dim(), 0);
            
            for (ui32 di=0; di<Data.size(); ++di) {
                while (listIndices[di] < Data[di].Values.size()) {
                    const double& newSpike = Data[di].Values[ listIndices[di] ];
                    if (newSpike <= (winStart + winLength)) { // in a window
                        window[di] += 1.0;
                        ++listIndices[di];
                    } else {
                        weAreDone = false;
                        break;
                    }
                }
            }

            for (ui32 di=0; di<Data.size(); ++di) {
                out.AddValue(di, static_cast<double>(window[di])/winLength);
            }
            winStart += winLength;
            if ((timeEnd > 0) && (winStart < timeEnd) && (weAreDone)) {
                weAreDone = false;
            }
        }
        ui32 prevLabelTo = 0;
        for (const auto& lt: Info.Labels) {
            ui32 labelFrom = std::max(prevLabelTo, static_cast<ui32>(floor(lt.From/winLength)));
            ui32 labelTo = ceil(lt.To/winLength);
            ENSURE(labelFrom < labelTo, 
                "Failed to deduce new label borders: (" << lt.From << ", " << lt.To << ") -> (" << labelFrom << ", " << labelTo << ")");
            out.Info.AddLabelAtPos(Info.UniqueLabelNames.at(lt.LabelId), labelFrom, labelTo-labelFrom);
            prevLabelTo = labelTo;
        }

        return out;
    }

} // namespace NGroundProto