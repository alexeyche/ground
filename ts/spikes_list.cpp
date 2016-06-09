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
            for (const auto &spike_time: Data[di].Values) {
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

    TTimeSeries TSpikesList::ConvertToRateVectors(double winLength) const {
        TTimeSeries out;

        out.SetDimSize(Dim());
        
        double minSpikeTime = std::numeric_limits<double>::max();
        
        for (ui32 di=0; di<Data.size(); ++di) {
            if (Data[di].Values.size() > 0) {
                minSpikeTime = std::min(minSpikeTime, Data[di].Values.front());
            }
        }
        
        TVector<ui32> listIndices(Dim(), 0);
        
        TDeque<TPair<double, double>> silenceDiff;
        silenceDiff.emplace_back(0.0, minSpikeTime);

        bool weAreDone = false;
        while (!weAreDone) {
            double minNextSpikeTime = std::numeric_limits<double>::max();
            TVector<ui32> window(Dim(), 0);

            for (ui32 di=0; di<Data.size(); ++di) {
                while (listIndices[di] < Data[di].Values.size()) {
                    const double& newSpike = Data[di].Values[ listIndices[di] ];
                    if (newSpike <= (minSpikeTime + winLength)) { // in a window
                        window[di] += 1;
                    } else {
                        minNextSpikeTime = std::min(minNextSpikeTime, newSpike);
                        break;
                    }
                    ++listIndices[di];
                }
            }
            for (ui32 di=0; di<Data.size(); ++di) {
                out.AddValue(di, static_cast<double>(window[di])/winLength);
            }

            if (minNextSpikeTime == std::numeric_limits<double>::max()) {
                weAreDone = true;
            } else {
                silenceDiff.emplace_back(minSpikeTime, minNextSpikeTime);
                minSpikeTime = minNextSpikeTime;
            }
        }

        double cumulativeShift = 0.0;
        for (const auto& lt: Info.Labels) {
            double from = lt.From;
            double to = lt.To;
            L_INFO << "Working with " << "(" << from << ", " << to << ")";
            
            from -= cumulativeShift;
            to -= cumulativeShift;
            
            L_INFO << "After cumulative shift " << "(" << from << ", " << to << ")";
            while (silenceDiff.size() > 0) {
                const TPair<double, double>& head = silenceDiff.front();
                if (lt.From > head.second) {
                    double diff = head.second - head.first;
                    L_INFO << "(" << from << ", " << to << ") -> " << "(" << from - diff << ", " << to - diff << ")";
                    from -= diff;
                    to -= diff;
                    cumulativeShift += diff;
                    silenceDiff.pop_front();
                } else {
                    break;
                }
            }
            from = ceil(from/winLength);
            to = ceil(to/winLength);
            L_INFO << "Window ceiling: (" << from << ", " << to << ")";
            const TString& srcLabel = Info.UniqueLabelNames.at(lt.LabelId);
            L_INFO << "So adding " << "(" << from << ", " << to << ") duration " << to - from; 
            out.Info.AddLabelAtPos(srcLabel, from, to - from);
        }
        
        return out;
    }

} // namespace NGroundProto