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

} // namespace NGroundProto