#pragma once

#include <ground/base/base.h>
#include <ground/base/errors.h>

#include <ground/protos/extension.pb.h>
#include <ground/string.h>

#include <vector>
#include <map>
#include <iostream>
#include <set>

namespace NGround {


    template <typename Proto>
    class TProtoOptions {
    public:
        TProtoOptions(int argc, const char** argv, TString description, std::set<int> fields = {}) : Description(description) {
            for(size_t i=1; i<argc; ++i) {
                Opts.push_back(argv[i]);
            }

            const google::protobuf::Descriptor* descriptor = Proto::descriptor();
            int count = descriptor->field_count();
            for (int i = 0; i < count; ++i) {
                const google::protobuf::FieldDescriptor* fieldDesc = descriptor->field(i);
                if ((fields.size() > 0) && (fields.find(fieldDesc->number()) == fields.end())) {
                    continue;
                }
                TString long_option = NStr::CamelCaseToOption(fieldDesc->name());

                NamedFields.insert(std::make_pair(long_option, fieldDesc));

                TString short_option_v = fieldDesc->options().GetExtension(short_option);
                if(!short_option_v.empty()) {
                    ShortNamedFields.insert(std::make_pair(short_option_v, fieldDesc));
                }
                TString default_value_v = fieldDesc->options().GetExtension(default_value);
                if(!default_value_v.empty()) {
                    Defaults.push_back(std::make_pair(fieldDesc, default_value_v));
                }
                Fields.push_back(fieldDesc);
            }
        }

        bool Parse(Proto& message) const {
            if(Opts.size() == 0) {
                Usage();
                return false;
            }

            for(const auto& o: Opts) {
                if((o == "-h")||(o == "--help")) {
                    Usage();
                    return false;
                }
            }
            std::vector<TString> extraOpts;
            for(auto iter=Opts.begin(); iter != Opts.end(); ++iter) {
                {
                    auto ptr = NamedFields.find(*iter);
                    if(ptr != NamedFields.end()) {
                        SetMessageValue(message, ptr->second, iter, Opts.end());
                        continue;
                    }
                }
                {
                    auto ptr = ShortNamedFields.find(*iter);
                    if(ptr != ShortNamedFields.end()) {
                        SetMessageValue(message, ptr->second, iter, Opts.end());
                        continue;
                    }
                }
                extraOpts.push_back(*iter);
            }
            if(extraOpts.size()>0) {
                TString extraOptsStr;
                for(const auto& o: extraOpts) {
                    extraOptsStr += o + ", ";
                }
                throw TErrException() << "Got unknown options: " << extraOptsStr;
            }
            for(const auto &defs: Defaults) {
                const google::protobuf::Reflection* reflection = message.GetReflection();
                if(!reflection->HasField(message, defs.first)) {
                    std::vector<TString> v = {defs.first->name(), defs.second};
                    auto vIter = v.cbegin();
                    SetMessageValue(message, defs.first, vIter, v.end());
                }
            }
            return true;
        }

        void SetMessageValue(Proto& message, const google::protobuf::FieldDescriptor* desc,
                std::vector<TString>::const_iterator &value_iterator, std::vector<TString>::const_iterator end_iter) const {
            ENSURE(value_iterator != end_iter, "Got end of options");

            const google::protobuf::Reflection* reflection = message.GetReflection();
            switch(desc->type()) {
                case google::protobuf::FieldDescriptor::TYPE_BOOL:
                    reflection->SetBool(&message, desc, true);
                    break;
                case google::protobuf::FieldDescriptor::TYPE_UINT32:
                    {
                        ++value_iterator;
                        if(value_iterator == end_iter) {
                            throw TErrException() << "Can't find value for option " << desc->name();
                        }
                        reflection->SetUInt32(&message, desc, NStr::As<ui32>(*value_iterator));
                    }
                    break;
                case google::protobuf::FieldDescriptor::TYPE_STRING:
                    {
                        ++value_iterator;
                        if(value_iterator == end_iter) {
                            throw TErrException() << "Can't find value for option " << desc->name();
                        }
                        reflection->SetString(&message, desc, *value_iterator);
                    }
                    break;
                case google::protobuf::FieldDescriptor::TYPE_DOUBLE:
                    {
                        ++value_iterator;
                        if(value_iterator == end_iter) {
                            throw TErrException() << "Can't find value for option " << desc->name();
                        }
                        reflection->SetDouble(&message, desc, NStr::As<double>(*value_iterator));
                    }
                    break;
                default:
                    throw TErrException() << "Unknown protobuf type: " << desc->type();
            }

        }

        void Usage() const {
            std::cout << Description << "\n\n";
            const google::protobuf::Descriptor* descriptor = Proto::descriptor();
            int count = descriptor->field_count();
            for (const auto* fieldDesc: Fields) {
                std::cout << "\t" << NStr::CamelCaseToOption(fieldDesc->name());

                TString short_option_v = fieldDesc->options().GetExtension(short_option);
                if(!short_option_v.empty()) {
                    std::cout << ", " << short_option_v;
                }

                TString desc = fieldDesc->options().GetExtension(description);
                if(!desc.empty()) {
                    std::cout << "\n\t\t" << desc;
                }
                TString def = fieldDesc->options().GetExtension(default_value);
                if(!def.empty()) {
                    std::cout << ", default: " << def;
                }
                std::cout << "\n\n";
            }
        }

    private:
        std::vector<TString> Opts;

        std::vector<const google::protobuf::FieldDescriptor*> Fields;

        std::map<TString, const google::protobuf::FieldDescriptor*> NamedFields;
        std::map<TString, const google::protobuf::FieldDescriptor*> ShortNamedFields;
        std::vector<std::pair<const google::protobuf::FieldDescriptor*, TString>> Defaults;

        std::string Description;
    };



} // NDnn