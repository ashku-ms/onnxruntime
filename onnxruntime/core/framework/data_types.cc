// Copyright (c) Microsoft Corporation. All rights reserved.
// Licensed under the MIT License.

#include "core/framework/data_types.h"
#include "core/framework/tensor.h"
#include "core/graph/onnx_protobuf.h"

#ifdef __GNUC__
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"
#pragma GCC diagnostic ignored "-Wunused-parameter"
#endif
#include "onnx/defs/data_type_utils.h"
#ifdef __GNUC__
#pragma GCC diagnostic pop
#endif

using namespace ONNX_NAMESPACE;
namespace onnxruntime {

template <>
MLDataType DataTypeImpl::GetType<Tensor>() {
  return TensorTypeBase::Type();
}

static bool IsTensorTypeScalar(const ONNX_NAMESPACE::TypeProto_Tensor& tensor_type_proto) {
  int sz = tensor_type_proto.shape().dim_size();
  return sz == 0 || sz == 1;
}

namespace data_types_internal {

template <typename T>
constexpr ONNX_NAMESPACE::TensorProto_DataType ToTensorDataType();

template <>
constexpr ONNX_NAMESPACE::TensorProto_DataType ToTensorDataType<float>() {
  return ONNX_NAMESPACE::TensorProto_DataType_FLOAT;
}
template <>
constexpr ONNX_NAMESPACE::TensorProto_DataType ToTensorDataType<uint8_t>() {
  return ONNX_NAMESPACE::TensorProto_DataType_UINT8;
};
template <>
constexpr ONNX_NAMESPACE::TensorProto_DataType ToTensorDataType<int8_t>() {
  return ONNX_NAMESPACE::TensorProto_DataType_INT8;
};
template <>
constexpr ONNX_NAMESPACE::TensorProto_DataType ToTensorDataType<uint16_t>() {
  return ONNX_NAMESPACE::TensorProto_DataType_UINT16;
};
template <>
constexpr ONNX_NAMESPACE::TensorProto_DataType ToTensorDataType<int16_t>() {
  return ONNX_NAMESPACE::TensorProto_DataType_INT16;
};
template <>
constexpr ONNX_NAMESPACE::TensorProto_DataType ToTensorDataType<int32_t>() {
  return ONNX_NAMESPACE::TensorProto_DataType_INT32;
};
template <>
constexpr ONNX_NAMESPACE::TensorProto_DataType ToTensorDataType<int64_t>() {
  return ONNX_NAMESPACE::TensorProto_DataType_INT64;
};
template <>
constexpr ONNX_NAMESPACE::TensorProto_DataType ToTensorDataType<std::string>() {
  return ONNX_NAMESPACE::TensorProto_DataType_STRING;
};
template <>
constexpr ONNX_NAMESPACE::TensorProto_DataType ToTensorDataType<bool>() {
  return ONNX_NAMESPACE::TensorProto_DataType_BOOL;
};
template <>
constexpr ONNX_NAMESPACE::TensorProto_DataType ToTensorDataType<MLFloat16>() {
  return ONNX_NAMESPACE::TensorProto_DataType_FLOAT16;
};
template <>
constexpr ONNX_NAMESPACE::TensorProto_DataType ToTensorDataType<double>() {
  return ONNX_NAMESPACE::TensorProto_DataType_DOUBLE;
};
template <>
constexpr ONNX_NAMESPACE::TensorProto_DataType ToTensorDataType<uint32_t>() {
  return ONNX_NAMESPACE::TensorProto_DataType_UINT32;
};
template <>
constexpr ONNX_NAMESPACE::TensorProto_DataType ToTensorDataType<uint64_t>() {
  return ONNX_NAMESPACE::TensorProto_DataType_UINT64;
};

template <typename T>
struct TensorContainedTypeSetter<T> {
  static void SetTensorElementType(ONNX_NAMESPACE::TypeProto& proto) {
    proto.mutable_tensor_type()->set_elem_type(ToTensorDataType<T>());
  }
  static void SetMapKeyType(ONNX_NAMESPACE::TypeProto& proto) {
    proto.mutable_map_type()->set_key_type(ToTensorDataType<T>());
  }
};

// Pre-instantiate
template struct
    TensorContainedTypeSetter<float>;
template struct
    TensorContainedTypeSetter<uint8_t>;
template struct
    TensorContainedTypeSetter<int8_t>;
template struct
    TensorContainedTypeSetter<uint16_t>;
template struct
    TensorContainedTypeSetter<int16_t>;
template struct
    TensorContainedTypeSetter<int32_t>;
template struct
    TensorContainedTypeSetter<int64_t>;
template struct
    TensorContainedTypeSetter<std::string>;
template struct
    TensorContainedTypeSetter<bool>;
template struct
    TensorContainedTypeSetter<MLFloat16>;
template struct
    TensorContainedTypeSetter<double>;
template struct
    TensorContainedTypeSetter<uint32_t>;
template struct
    TensorContainedTypeSetter<uint64_t>;

void CopyMutableMapValue(const ONNX_NAMESPACE::TypeProto& value_proto,
                         ONNX_NAMESPACE::TypeProto& map_proto) {
  map_proto.mutable_map_type()->mutable_value_type()->CopyFrom(value_proto);
}

void CopyMutableSeqElement(const ONNX_NAMESPACE::TypeProto& elem_proto,
                           ONNX_NAMESPACE::TypeProto& proto) {
  proto.mutable_sequence_type()->mutable_elem_type()->CopyFrom(elem_proto);
}

void AssignOpaqueDomainName(const char* domain, const char* name,
                            ONNX_NAMESPACE::TypeProto& proto) {
  auto* mutable_opaque = proto.mutable_opaque_type();
  mutable_opaque->mutable_domain()->assign(domain);
  mutable_opaque->mutable_name()->assign(name);
}

bool IsCompatible(const ONNX_NAMESPACE::TypeProto_Tensor& tensor_proto,
                  const ONNX_NAMESPACE::TypeProto_Tensor& type_proto);

bool IsCompatible(const ONNX_NAMESPACE::TypeProto_SparseTensor& tensor_proto,
                  const ONNX_NAMESPACE::TypeProto_SparseTensor& type_proto);

bool IsCompatible(const ONNX_NAMESPACE::TypeProto_Map& map_proto,
                  const ONNX_NAMESPACE::TypeProto_Map& type_proto);

bool IsCompatible(const ONNX_NAMESPACE::TypeProto_Sequence& sequence_proto,
                  const ONNX_NAMESPACE::TypeProto_Sequence& type_proto);

bool IsCompatible(const ONNX_NAMESPACE::TypeProto_Opaque& opaque_proto,
                  const ONNX_NAMESPACE::TypeProto_Opaque& type_proto);

bool IsCompatible(const ONNX_NAMESPACE::TypeProto_Tensor& tensor_proto,
                  const ONNX_NAMESPACE::TypeProto_Tensor& type_proto) {
  return type_proto.has_elem_type() &&
         type_proto.elem_type() == tensor_proto.elem_type();
  /* Currently all Tensors with all kinds of shapes
     are mapped into the same MLDataType (same element type)
     so we omit shape from IsCompatible consideration
     */
}

bool IsCompatible(const ONNX_NAMESPACE::TypeProto_Map& map_proto,
                  const ONNX_NAMESPACE::TypeProto_Map& type_proto) {
  if (!(type_proto.has_key_type() &&
        type_proto.key_type() == map_proto.key_type())) {
    return false;
  }
  const auto& lhs = map_proto;
  const auto& rhs = type_proto;
  bool result = true;
  if (lhs.key_type() == rhs.key_type() &&
      lhs.value_type().value_case() == rhs.value_type().value_case()) {
    switch (lhs.value_type().value_case()) {
      case TypeProto::ValueCase::kTensorType:
        result = IsCompatible(lhs.value_type().tensor_type(), rhs.value_type().tensor_type());
        break;
      case TypeProto::ValueCase::kSequenceType:
        result = IsCompatible(lhs.value_type().sequence_type(), rhs.value_type().sequence_type());
        break;
      case TypeProto::ValueCase::kMapType:
        result = IsCompatible(lhs.value_type().map_type(), rhs.value_type().map_type());
        break;
      case TypeProto::ValueCase::kOpaqueType:
        result = IsCompatible(lhs.value_type().opaque_type(), rhs.value_type().opaque_type());
        break;
      case TypeProto::ValueCase::kSparseTensorType:
        result = IsCompatible(lhs.value_type().sparse_tensor_type(), rhs.value_type().sparse_tensor_type());
        break;
      default:
        ONNXRUNTIME_ENFORCE(false);
        break;
    }
  } else {
    result = false;
  }
  return result;
}

bool IsCompatible(const ONNX_NAMESPACE::TypeProto_Sequence& sequence_proto,
                  const ONNX_NAMESPACE::TypeProto_Sequence& type_proto) {
  bool result = true;
  const auto& lhs = sequence_proto;
  const auto& rhs = type_proto;
  if (rhs.has_elem_type() &&
      lhs.elem_type().value_case() == rhs.elem_type().value_case()) {
    switch (lhs.elem_type().value_case()) {
      case TypeProto::ValueCase::kTensorType:
        result = IsCompatible(lhs.elem_type().tensor_type(), rhs.elem_type().tensor_type());
        break;
      case TypeProto::ValueCase::kSequenceType:
        result = IsCompatible(lhs.elem_type().sequence_type(), rhs.elem_type().sequence_type());
        break;
      case TypeProto::ValueCase::kMapType:
        result = IsCompatible(lhs.elem_type().map_type(), rhs.elem_type().map_type());
        break;
      case TypeProto::ValueCase::kOpaqueType:
        result = IsCompatible(lhs.elem_type().opaque_type(), rhs.elem_type().opaque_type());
        break;
      case TypeProto::ValueCase::kSparseTensorType:
        result = IsCompatible(lhs.elem_type().sparse_tensor_type(), rhs.elem_type().sparse_tensor_type());
        break;
      default:
        ONNXRUNTIME_ENFORCE(false);
        break;
    }
  } else {
    result = false;
  }
  return result;
}
bool IsCompatible(const ONNX_NAMESPACE::TypeProto_Opaque& opaque_proto, const ONNX_NAMESPACE::TypeProto_Opaque& type_proto) {
  const auto& lhs = opaque_proto;
  const auto& rhs = type_proto;
  bool lhs_domain = lhs.has_domain() && !lhs.domain().empty();
  bool rhs_domain = rhs.has_domain() && !rhs.domain().empty();

  if ((lhs_domain != rhs_domain) ||
      (lhs_domain && rhs_domain && lhs.domain() != lhs.domain())) {
    return false;
  }

  bool lhs_name = lhs.has_name() && !lhs.name().empty();
  bool rhs_name = rhs.has_name() && !rhs.name().empty();

  return !((lhs_name != rhs_name) ||
           (lhs_name && rhs_name && lhs.name() != rhs.name()));
}

bool IsCompatible(const ONNX_NAMESPACE::TypeProto_SparseTensor& tensor_proto,
                  const ONNX_NAMESPACE::TypeProto_SparseTensor& type_proto) {
  return type_proto.has_elem_type() &&
         type_proto.elem_type() == tensor_proto.elem_type();
  // XXX: Ignoring shape for now
}

void RegisterAllProtos(const std::function<void(MLDataType)>& /*reg_fn*/);

class DataTypeRegistry {
  std::unordered_map<DataType, MLDataType> mapping_;

  DataTypeRegistry() {
    RegisterAllProtos([this](MLDataType mltype) { RegisterDataType(mltype); });
  }

  ~DataTypeRegistry() = default;

 public:
  DataTypeRegistry(const DataTypeRegistry&) = delete;
  DataTypeRegistry& operator=(const DataTypeRegistry&) = delete;

  static DataTypeRegistry& instance() {
    static DataTypeRegistry inst;
    return inst;
  }

  void RegisterDataType(MLDataType mltype) {
    using namespace ONNX_NAMESPACE;
    const auto* proto = mltype->GetTypeProto();
    ONNXRUNTIME_ENFORCE(proto != nullptr, "Only ONNX MLDataType can be registered");
    DataType type = Utils::DataTypeUtils::ToType(*proto);
    auto p = mapping_.insert(std::make_pair(type, mltype));
    ONNXRUNTIME_ENFORCE(p.second, "We do not expect duplicate registration of types for: ", type);
  }

  MLDataType GetMLDataType(const ONNX_NAMESPACE::TypeProto& proto) const {
    using namespace ONNX_NAMESPACE;
    DataType type = Utils::DataTypeUtils::ToType(proto);
    auto p = mapping_.find(type);
    if (p != mapping_.end()) {
      return p->second;
    }
    return nullptr;
  }
};

struct TypeProtoImpl {
  const TypeProto* GetProto() const {
    return &proto_;
  }
  TypeProto& mutable_type_proto() {
    return proto_;
  }

  TypeProto proto_;
};

}  // namespace data_types_internal

/// TensorTypeBase
struct TensorTypeBase::Impl : public data_types_internal::TypeProtoImpl {
};

const ONNX_NAMESPACE::TypeProto* TensorTypeBase::GetTypeProto() const {
  return impl_->GetProto();
}

TensorTypeBase::TensorTypeBase() : impl_(new Impl()) {}
TensorTypeBase::~TensorTypeBase() {
  delete impl_;
}

size_t TensorTypeBase::Size() const {
  return sizeof(Tensor);
}

template <typename T>
static void Delete(void* p) {
  delete static_cast<T*>(p);
}

DeleteFunc TensorTypeBase::GetDeleteFunc() const {
  return &Delete<Tensor>;
}

ONNX_NAMESPACE::TypeProto& TensorTypeBase::mutable_type_proto() {
  return impl_->mutable_type_proto();
}

bool TensorTypeBase::IsCompatible(const ONNX_NAMESPACE::TypeProto& type_proto) const {
  const auto* thisProto = GetTypeProto();
  if (&type_proto == thisProto) {
    return true;
  }
  if (type_proto.value_case() != TypeProto::ValueCase::kTensorType) {
    return false;
  }

  ONNXRUNTIME_ENFORCE(thisProto->value_case() == TypeProto::ValueCase::kTensorType);
  ONNXRUNTIME_ENFORCE(thisProto->tensor_type().has_elem_type());

  return data_types_internal::IsCompatible(thisProto->tensor_type(), type_proto.tensor_type());
}

MLDataType TensorTypeBase::Type() {
  static TensorTypeBase tensor_base;
  return &tensor_base;
}

/// NoTensorTypeBase
struct NonTensorTypeBase::Impl : public data_types_internal::TypeProtoImpl {};

NonTensorTypeBase::NonTensorTypeBase() : impl_(new Impl()) {
}

NonTensorTypeBase::~NonTensorTypeBase() {
  delete impl_;
}

ONNX_NAMESPACE::TypeProto& NonTensorTypeBase::mutable_type_proto() {
  return impl_->mutable_type_proto();
}

const ONNX_NAMESPACE::TypeProto* NonTensorTypeBase::GetTypeProto() const {
  return impl_->GetProto();
}

bool NonTensorTypeBase::IsMapCompatible(const ONNX_NAMESPACE::TypeProto& type_proto) const {
  const auto* thisProto = impl_->GetProto();
  if (&type_proto == thisProto) {
    return true;
  }
  if (type_proto.value_case() != TypeProto::ValueCase::kMapType) {
    return false;
  }
  ONNXRUNTIME_ENFORCE(thisProto->value_case() == TypeProto::ValueCase::kMapType);
  ONNXRUNTIME_ENFORCE(thisProto->map_type().has_key_type());
  ONNXRUNTIME_ENFORCE(thisProto->map_type().has_value_type());
  return data_types_internal::IsCompatible(thisProto->map_type(), type_proto.map_type());
}

bool NonTensorTypeBase::IsSequenceCompatible(const ONNX_NAMESPACE::TypeProto& type_proto) const {
  const auto* thisProto = impl_->GetProto();
  if (&type_proto == thisProto) {
    return true;
  }
  if (type_proto.value_case() != TypeProto::ValueCase::kSequenceType) {
    return false;
  }
  ONNXRUNTIME_ENFORCE(thisProto->value_case() == TypeProto::ValueCase::kSequenceType);
  ONNXRUNTIME_ENFORCE(thisProto->sequence_type().has_elem_type());
  return data_types_internal::IsCompatible(thisProto->sequence_type(), type_proto.sequence_type());
}

bool NonTensorTypeBase::IsOpaqueCompatible(const ONNX_NAMESPACE::TypeProto& type_proto) const {
  const auto* thisProto = impl_->GetProto();
  if (&type_proto == thisProto) {
    return true;
  }
  if (type_proto.value_case() != TypeProto::ValueCase::kOpaqueType) {
    return false;
  }
  ONNXRUNTIME_ENFORCE(thisProto->value_case() == TypeProto::ValueCase::kOpaqueType);
  ONNXRUNTIME_ENFORCE(thisProto->opaque_type().has_domain());
  ONNXRUNTIME_ENFORCE(thisProto->opaque_type().has_name());
  return data_types_internal::IsCompatible(thisProto->opaque_type(), type_proto.opaque_type());
}

ONNXRUNTIME_REGISTER_TENSOR_TYPE(int32_t);
ONNXRUNTIME_REGISTER_TENSOR_TYPE(float);
ONNXRUNTIME_REGISTER_TENSOR_TYPE(bool);
ONNXRUNTIME_REGISTER_TENSOR_TYPE(std::string);
ONNXRUNTIME_REGISTER_TENSOR_TYPE(int8_t);
ONNXRUNTIME_REGISTER_TENSOR_TYPE(uint8_t);
ONNXRUNTIME_REGISTER_TENSOR_TYPE(uint16_t);
ONNXRUNTIME_REGISTER_TENSOR_TYPE(int16_t);
ONNXRUNTIME_REGISTER_TENSOR_TYPE(int64_t);
ONNXRUNTIME_REGISTER_TENSOR_TYPE(double);
ONNXRUNTIME_REGISTER_TENSOR_TYPE(uint32_t);
ONNXRUNTIME_REGISTER_TENSOR_TYPE(uint64_t);
ONNXRUNTIME_REGISTER_TENSOR_TYPE(MLFloat16);

ONNXRUNTIME_REGISTER_MAP(MapStringToString);
ONNXRUNTIME_REGISTER_MAP(MapStringToInt64);
ONNXRUNTIME_REGISTER_MAP(MapStringToFloat);
ONNXRUNTIME_REGISTER_MAP(MapStringToDouble);
ONNXRUNTIME_REGISTER_MAP(MapInt64ToString);
ONNXRUNTIME_REGISTER_MAP(MapInt64ToInt64);
ONNXRUNTIME_REGISTER_MAP(MapInt64ToFloat);
ONNXRUNTIME_REGISTER_MAP(MapInt64ToDouble);

ONNXRUNTIME_REGISTER_SEQ(VectorString);
ONNXRUNTIME_REGISTER_SEQ(VectorFloat);
ONNXRUNTIME_REGISTER_SEQ(VectorInt64);
ONNXRUNTIME_REGISTER_SEQ(VectorDouble);

ONNXRUNTIME_REGISTER_SEQ(VectorMapStringToFloat);
ONNXRUNTIME_REGISTER_SEQ(VectorMapInt64ToFloat);

// Used for Tensor Proto registrations
#define REGISTER_TENSOR_PROTO(TYPE, reg_fn)                  \
  {                                                          \
    MLDataType mltype = DataTypeImpl::GetTensorType<TYPE>(); \
    reg_fn(mltype);                                          \
  }

#define REGISTER_ONNX_PROTO(TYPE, reg_fn)              \
  {                                                    \
    MLDataType mltype = DataTypeImpl::GetType<TYPE>(); \
    reg_fn(mltype);                                    \
  }

namespace data_types_internal {

void RegisterAllProtos(const std::function<void(MLDataType)>& reg_fn) {
  REGISTER_TENSOR_PROTO(int32_t, reg_fn);
  REGISTER_TENSOR_PROTO(float, reg_fn);
  REGISTER_TENSOR_PROTO(bool, reg_fn);
  REGISTER_TENSOR_PROTO(std::string, reg_fn);
  REGISTER_TENSOR_PROTO(int8_t, reg_fn);
  REGISTER_TENSOR_PROTO(uint8_t, reg_fn);
  REGISTER_TENSOR_PROTO(uint16_t, reg_fn);
  REGISTER_TENSOR_PROTO(int16_t, reg_fn);
  REGISTER_TENSOR_PROTO(int64_t, reg_fn);
  REGISTER_TENSOR_PROTO(double, reg_fn);
  REGISTER_TENSOR_PROTO(uint32_t, reg_fn);
  REGISTER_TENSOR_PROTO(uint64_t, reg_fn);
  REGISTER_TENSOR_PROTO(MLFloat16, reg_fn);

  REGISTER_ONNX_PROTO(MapStringToString, reg_fn);
  REGISTER_ONNX_PROTO(MapStringToInt64, reg_fn);
  REGISTER_ONNX_PROTO(MapStringToFloat, reg_fn);
  REGISTER_ONNX_PROTO(MapStringToDouble, reg_fn);
  REGISTER_ONNX_PROTO(MapInt64ToString, reg_fn);
  REGISTER_ONNX_PROTO(MapInt64ToInt64, reg_fn);
  REGISTER_ONNX_PROTO(MapInt64ToFloat, reg_fn);
  REGISTER_ONNX_PROTO(MapInt64ToDouble, reg_fn);

  REGISTER_ONNX_PROTO(VectorString, reg_fn);
  REGISTER_ONNX_PROTO(VectorFloat, reg_fn);
  REGISTER_ONNX_PROTO(VectorInt64, reg_fn);
  REGISTER_ONNX_PROTO(VectorDouble, reg_fn);

  REGISTER_ONNX_PROTO(VectorMapStringToFloat, reg_fn);
  REGISTER_ONNX_PROTO(VectorMapInt64ToFloat, reg_fn);
}
}  // namespace data_types_internal

void DataTypeImpl::RegisterDataType(MLDataType mltype) {
  data_types_internal::DataTypeRegistry::instance().RegisterDataType(mltype);
}

MLDataType DataTypeImpl::TypeFromProto(const ONNX_NAMESPACE::TypeProto& proto) {
  const auto& registry = data_types_internal::DataTypeRegistry::instance();

  switch (proto.value_case()) {
    case TypeProto::ValueCase::kTensorType: {
      const auto& tensor_type = proto.tensor_type();
      ONNXRUNTIME_ENFORCE(tensor_type.has_elem_type());
      switch (tensor_type.elem_type()) {
        case TensorProto_DataType_FLOAT:
          return DataTypeImpl::GetTensorType<float>();
        case TensorProto_DataType_BOOL:
          return DataTypeImpl::GetTensorType<bool>();
        case TensorProto_DataType_INT32:
          return DataTypeImpl::GetTensorType<int32_t>();
        case TensorProto_DataType_DOUBLE:
          return DataTypeImpl::GetTensorType<double>();
        case TensorProto_DataType_STRING:
          return DataTypeImpl::GetTensorType<std::string>();
        case TensorProto_DataType_UINT8:
          return DataTypeImpl::GetTensorType<uint8_t>();
        case TensorProto_DataType_UINT16:
          return DataTypeImpl::GetTensorType<uint16_t>();
        case TensorProto_DataType_INT8:
          return DataTypeImpl::GetTensorType<int8_t>();
        case TensorProto_DataType_INT16:
          return DataTypeImpl::GetTensorType<int16_t>();
        case TensorProto_DataType_INT64:
          return DataTypeImpl::GetTensorType<int64_t>();
        case TensorProto_DataType_UINT32:
          return DataTypeImpl::GetTensorType<uint32_t>();
        case TensorProto_DataType_UINT64:
          return DataTypeImpl::GetTensorType<uint64_t>();
        case TensorProto_DataType_FLOAT16:
          return DataTypeImpl::GetTensorType<MLFloat16>();
        default:
          ONNXRUNTIME_NOT_IMPLEMENTED("tensor type ", tensor_type.elem_type(), " is not supported");
      }
    } break;  // kTensorType
    case TypeProto::ValueCase::kMapType: {
      const auto& maptype = proto.map_type();
      auto keytype = maptype.key_type();
      const auto& value_type = maptype.value_type();

      if (value_type.value_case() == TypeProto::ValueCase::kTensorType &&
          IsTensorTypeScalar(value_type.tensor_type())) {
        auto value_elem_type = value_type.tensor_type().elem_type();
        switch (value_elem_type) {
          case TensorProto_DataType_STRING: {
            switch (keytype) {
              case TensorProto_DataType_STRING:
                return DataTypeImpl::GetType<MapStringToString>();
              case TensorProto_DataType_INT64:
                return DataTypeImpl::GetType<MapInt64ToString>();
              default:
                break;
            }
          } break;
          case TensorProto_DataType_INT64:
            switch (keytype) {
              case TensorProto_DataType_STRING:
                return DataTypeImpl::GetType<MapStringToInt64>();
              case TensorProto_DataType_INT64:
                return DataTypeImpl::GetType<MapInt64ToInt64>();
              default:
                break;
            }
            break;
          case TensorProto_DataType_FLOAT:
            switch (keytype) {
              case TensorProto_DataType_STRING:
                return DataTypeImpl::GetType<MapStringToFloat>();
              case TensorProto_DataType_INT64:
                return DataTypeImpl::GetType<MapInt64ToFloat>();
              default:
                break;
            }
            break;
          case TensorProto_DataType_DOUBLE:
            switch (keytype) {
              case TensorProto_DataType_STRING:
                return DataTypeImpl::GetType<MapStringToDouble>();
              case TensorProto_DataType_INT64:
                return DataTypeImpl::GetType<MapInt64ToDouble>();
              default:
                break;
            }
            break;
          default:
            break;
        }
        MLDataType type = registry.GetMLDataType(proto);
        ONNXRUNTIME_ENFORCE(type != nullptr, "Map with key type: ", keytype, " value type: ", value_elem_type, " is not registered");
        return type;
      }  // not if(scalar tensor) pre-reg types
      MLDataType type = registry.GetMLDataType(proto);
      if (type == nullptr) {
        DataType str_type = ONNX_NAMESPACE::Utils::DataTypeUtils::ToType(proto);
        ONNXRUNTIME_NOT_IMPLEMENTED("type: ", *str_type, " is not registered");
      }
      return type;

    } break;  // kMapType
    case TypeProto::ValueCase::kSequenceType: {
      auto& seq_type = proto.sequence_type();
      auto& val_type = seq_type.elem_type();

      switch (val_type.value_case()) {
        case TypeProto::ValueCase::kMapType: {
          auto& maptype = val_type.map_type();
          auto keytype = maptype.key_type();
          auto& value_type = maptype.value_type();

          if (value_type.value_case() == TypeProto::ValueCase::kTensorType &&
              IsTensorTypeScalar(value_type.tensor_type())) {
            auto value_elem_type = value_type.tensor_type().elem_type();
            switch (value_elem_type) {
              case TensorProto_DataType_FLOAT: {
                switch (keytype) {
                  case TensorProto_DataType_STRING:
                    return DataTypeImpl::GetType<VectorMapStringToFloat>();
                  case TensorProto_DataType_INT64:
                    return DataTypeImpl::GetType<VectorMapInt64ToFloat>();
                  default:
                    break;
                }
              }
              default:
                break;
            }
          }
        }  // MapType
        break;
        case TypeProto::ValueCase::kTensorType: {
          auto val_elem_type = val_type.tensor_type().elem_type();
          switch (val_elem_type) {
            case TensorProto_DataType_STRING:
              return DataTypeImpl::GetType<VectorString>();
            case TensorProto_DataType_INT64:
              return DataTypeImpl::GetType<VectorInt64>();
            case TensorProto_DataType_FLOAT:
              return DataTypeImpl::GetType<VectorFloat>();
            case TensorProto_DataType_DOUBLE:
              return DataTypeImpl::GetType<VectorDouble>();
            default:
              break;
          }
        }  // kTensorType
        break;
        default:
          break;
      }  // Sequence value case
    }    // kSequenceType
    break;
    default:
      break;
  }  // proto.value_case()
  MLDataType type = registry.GetMLDataType(proto);
  if (type == nullptr) {
    DataType str_type = ONNX_NAMESPACE::Utils::DataTypeUtils::ToType(proto);
    ONNXRUNTIME_NOT_IMPLEMENTED("type: ", *str_type, " is not currently registered or supported");
  }
  return type;
}

//Below are the types the we need to execute the runtime
//They are not compatible with TypeProto in ONNX.
ONNXRUNTIME_REGISTER_NON_ONNX_TYPE(int32_t);
ONNXRUNTIME_REGISTER_NON_ONNX_TYPE(float);
ONNXRUNTIME_REGISTER_NON_ONNX_TYPE(bool);
ONNXRUNTIME_REGISTER_NON_ONNX_TYPE(std::string);
ONNXRUNTIME_REGISTER_NON_ONNX_TYPE(int8_t);
ONNXRUNTIME_REGISTER_NON_ONNX_TYPE(uint8_t);
ONNXRUNTIME_REGISTER_NON_ONNX_TYPE(uint16_t);
ONNXRUNTIME_REGISTER_NON_ONNX_TYPE(int16_t);
ONNXRUNTIME_REGISTER_NON_ONNX_TYPE(int64_t);
ONNXRUNTIME_REGISTER_NON_ONNX_TYPE(double);
ONNXRUNTIME_REGISTER_NON_ONNX_TYPE(uint32_t);
ONNXRUNTIME_REGISTER_NON_ONNX_TYPE(uint64_t);
ONNXRUNTIME_REGISTER_NON_ONNX_TYPE(MLFloat16);

const std::vector<MLDataType>& DataTypeImpl::AllFixedSizeTensorTypes() {
  static std::vector<MLDataType> all_fixed_size_tensor_types =
      {DataTypeImpl::GetTensorType<float>(),
       DataTypeImpl::GetTensorType<double>(),
       DataTypeImpl::GetTensorType<int64_t>(),
       DataTypeImpl::GetTensorType<uint64_t>(),
       DataTypeImpl::GetTensorType<int32_t>(),
       DataTypeImpl::GetTensorType<uint32_t>(),
       DataTypeImpl::GetTensorType<int16_t>(),
       DataTypeImpl::GetTensorType<uint16_t>(),
       DataTypeImpl::GetTensorType<int8_t>(),
       DataTypeImpl::GetTensorType<uint8_t>(),
       DataTypeImpl::GetTensorType<MLFloat16>(),
       DataTypeImpl::GetTensorType<bool>()};

  return all_fixed_size_tensor_types;
}

const std::vector<MLDataType>& DataTypeImpl::AllTensorTypes() {
  static std::vector<MLDataType> all_tensor_types =
      {DataTypeImpl::GetTensorType<float>(),
       DataTypeImpl::GetTensorType<double>(),
       DataTypeImpl::GetTensorType<int64_t>(),
       DataTypeImpl::GetTensorType<uint64_t>(),
       DataTypeImpl::GetTensorType<int32_t>(),
       DataTypeImpl::GetTensorType<uint32_t>(),
       DataTypeImpl::GetTensorType<int16_t>(),
       DataTypeImpl::GetTensorType<uint16_t>(),
       DataTypeImpl::GetTensorType<int8_t>(),
       DataTypeImpl::GetTensorType<uint8_t>(),
       DataTypeImpl::GetTensorType<MLFloat16>(),
       DataTypeImpl::GetTensorType<bool>(),
       DataTypeImpl::GetTensorType<std::string>()};

  return all_tensor_types;
}

// helper to stream. expected to only be used for error output, so any typeid lookup
// cost should be fine. alternative would be to add a static string field to DataTypeImpl
// that we set in the register macro to the type name, and output that instead.
std::ostream& operator<<(std::ostream& out, const DataTypeImpl* data_type) {
  if (data_type == nullptr)
    return out << "(null)";

  return out << typeid(*data_type).name();
}

}  // namespace onnxruntime
