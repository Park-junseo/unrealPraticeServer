// Generated by the protocol buffer compiler.  DO NOT EDIT!
// source: Protocol.proto

#include "Protocol.pb.h"

#include <algorithm>

#include <google/protobuf/io/coded_stream.h>
#include <google/protobuf/extension_set.h>
#include <google/protobuf/wire_format_lite.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/generated_message_reflection.h>
#include <google/protobuf/reflection_ops.h>
#include <google/protobuf/wire_format.h>
// @@protoc_insertion_point(includes)
#include <google/protobuf/port_def.inc>

PROTOBUF_PRAGMA_INIT_SEG

namespace _pb = ::PROTOBUF_NAMESPACE_ID;
namespace _pbi = _pb::internal;

namespace Protocol {
PROTOBUF_CONSTEXPR C_TEST::C_TEST(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_.id_)*/uint64_t{0u}
  , /*decltype(_impl_._cached_size_)*/{}} {}
struct C_TESTDefaultTypeInternal {
  PROTOBUF_CONSTEXPR C_TESTDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~C_TESTDefaultTypeInternal() {}
  union {
    C_TEST _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 C_TESTDefaultTypeInternal _C_TEST_default_instance_;
PROTOBUF_CONSTEXPR S_TEST::S_TEST(
    ::_pbi::ConstantInitialized): _impl_{
    /*decltype(_impl_.buffs_)*/{}
  , /*decltype(_impl_.id_)*/uint64_t{0u}
  , /*decltype(_impl_.hp_)*/0u
  , /*decltype(_impl_.attack_)*/0u
  , /*decltype(_impl_._cached_size_)*/{}} {}
struct S_TESTDefaultTypeInternal {
  PROTOBUF_CONSTEXPR S_TESTDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~S_TESTDefaultTypeInternal() {}
  union {
    S_TEST _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 S_TESTDefaultTypeInternal _S_TEST_default_instance_;
PROTOBUF_CONSTEXPR C_MOVE::C_MOVE(
    ::_pbi::ConstantInitialized) {}
struct C_MOVEDefaultTypeInternal {
  PROTOBUF_CONSTEXPR C_MOVEDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~C_MOVEDefaultTypeInternal() {}
  union {
    C_MOVE _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 C_MOVEDefaultTypeInternal _C_MOVE_default_instance_;
PROTOBUF_CONSTEXPR S_LOGIN::S_LOGIN(
    ::_pbi::ConstantInitialized) {}
struct S_LOGINDefaultTypeInternal {
  PROTOBUF_CONSTEXPR S_LOGINDefaultTypeInternal()
      : _instance(::_pbi::ConstantInitialized{}) {}
  ~S_LOGINDefaultTypeInternal() {}
  union {
    S_LOGIN _instance;
  };
};
PROTOBUF_ATTRIBUTE_NO_DESTROY PROTOBUF_CONSTINIT PROTOBUF_ATTRIBUTE_INIT_PRIORITY1 S_LOGINDefaultTypeInternal _S_LOGIN_default_instance_;
}  // namespace Protocol
static ::_pb::Metadata file_level_metadata_Protocol_2eproto[4];
static constexpr ::_pb::EnumDescriptor const** file_level_enum_descriptors_Protocol_2eproto = nullptr;
static constexpr ::_pb::ServiceDescriptor const** file_level_service_descriptors_Protocol_2eproto = nullptr;

const uint32_t TableStruct_Protocol_2eproto::offsets[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Protocol::C_TEST, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::Protocol::C_TEST, _impl_.id_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Protocol::S_TEST, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  PROTOBUF_FIELD_OFFSET(::Protocol::S_TEST, _impl_.id_),
  PROTOBUF_FIELD_OFFSET(::Protocol::S_TEST, _impl_.hp_),
  PROTOBUF_FIELD_OFFSET(::Protocol::S_TEST, _impl_.attack_),
  PROTOBUF_FIELD_OFFSET(::Protocol::S_TEST, _impl_.buffs_),
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Protocol::C_MOVE, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
  ~0u,  // no _has_bits_
  PROTOBUF_FIELD_OFFSET(::Protocol::S_LOGIN, _internal_metadata_),
  ~0u,  // no _extensions_
  ~0u,  // no _oneof_case_
  ~0u,  // no _weak_field_map_
  ~0u,  // no _inlined_string_donated_
};
static const ::_pbi::MigrationSchema schemas[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) = {
  { 0, -1, -1, sizeof(::Protocol::C_TEST)},
  { 7, -1, -1, sizeof(::Protocol::S_TEST)},
  { 17, -1, -1, sizeof(::Protocol::C_MOVE)},
  { 23, -1, -1, sizeof(::Protocol::S_LOGIN)},
};

static const ::_pb::Message* const file_default_instances[] = {
  &::Protocol::_C_TEST_default_instance_._instance,
  &::Protocol::_S_TEST_default_instance_._instance,
  &::Protocol::_C_MOVE_default_instance_._instance,
  &::Protocol::_S_LOGIN_default_instance_._instance,
};

const char descriptor_table_protodef_Protocol_2eproto[] PROTOBUF_SECTION_VARIABLE(protodesc_cold) =
  "\n\016Protocol.proto\022\010Protocol\032\nEnum.proto\032\014"
  "Struct.proto\"\024\n\006C_TEST\022\n\n\002id\030\001 \001(\004\"S\n\006S_"
  "TEST\022\n\n\002id\030\001 \001(\004\022\n\n\002hp\030\002 \001(\r\022\016\n\006attack\030\003"
  " \001(\r\022!\n\005buffs\030\004 \003(\0132\022.Protocol.BuffData\""
  "\010\n\006C_MOVE\"\t\n\007S_LOGINb\006proto3"
  ;
static const ::_pbi::DescriptorTable* const descriptor_table_Protocol_2eproto_deps[2] = {
  &::descriptor_table_Enum_2eproto,
  &::descriptor_table_Struct_2eproto,
};
static ::_pbi::once_flag descriptor_table_Protocol_2eproto_once;
const ::_pbi::DescriptorTable descriptor_table_Protocol_2eproto = {
    false, false, 188, descriptor_table_protodef_Protocol_2eproto,
    "Protocol.proto",
    &descriptor_table_Protocol_2eproto_once, descriptor_table_Protocol_2eproto_deps, 2, 4,
    schemas, file_default_instances, TableStruct_Protocol_2eproto::offsets,
    file_level_metadata_Protocol_2eproto, file_level_enum_descriptors_Protocol_2eproto,
    file_level_service_descriptors_Protocol_2eproto,
};
PROTOBUF_ATTRIBUTE_WEAK const ::_pbi::DescriptorTable* descriptor_table_Protocol_2eproto_getter() {
  return &descriptor_table_Protocol_2eproto;
}

// Force running AddDescriptors() at dynamic initialization time.
PROTOBUF_ATTRIBUTE_INIT_PRIORITY2 static ::_pbi::AddDescriptorsRunner dynamic_init_dummy_Protocol_2eproto(&descriptor_table_Protocol_2eproto);
namespace Protocol {

// ===================================================================

class C_TEST::_Internal {
 public:
};

C_TEST::C_TEST(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:Protocol.C_TEST)
}
C_TEST::C_TEST(const C_TEST& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  C_TEST* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_.id_){}
    , /*decltype(_impl_._cached_size_)*/{}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  _this->_impl_.id_ = from._impl_.id_;
  // @@protoc_insertion_point(copy_constructor:Protocol.C_TEST)
}

inline void C_TEST::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_.id_){uint64_t{0u}}
    , /*decltype(_impl_._cached_size_)*/{}
  };
}

C_TEST::~C_TEST() {
  // @@protoc_insertion_point(destructor:Protocol.C_TEST)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void C_TEST::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
}

void C_TEST::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void C_TEST::Clear() {
// @@protoc_insertion_point(message_clear_start:Protocol.C_TEST)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.id_ = uint64_t{0u};
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* C_TEST::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // uint64 id = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 8)) {
          _impl_.id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* C_TEST::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:Protocol.C_TEST)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // uint64 id = 1;
  if (this->_internal_id() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteUInt64ToArray(1, this->_internal_id(), target);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Protocol.C_TEST)
  return target;
}

size_t C_TEST::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Protocol.C_TEST)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // uint64 id = 1;
  if (this->_internal_id() != 0) {
    total_size += ::_pbi::WireFormatLite::UInt64SizePlusOne(this->_internal_id());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData C_TEST::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    C_TEST::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*C_TEST::GetClassData() const { return &_class_data_; }


void C_TEST::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<C_TEST*>(&to_msg);
  auto& from = static_cast<const C_TEST&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:Protocol.C_TEST)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  if (from._internal_id() != 0) {
    _this->_internal_set_id(from._internal_id());
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void C_TEST::CopyFrom(const C_TEST& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Protocol.C_TEST)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool C_TEST::IsInitialized() const {
  return true;
}

void C_TEST::InternalSwap(C_TEST* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  swap(_impl_.id_, other->_impl_.id_);
}

::PROTOBUF_NAMESPACE_ID::Metadata C_TEST::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_Protocol_2eproto_getter, &descriptor_table_Protocol_2eproto_once,
      file_level_metadata_Protocol_2eproto[0]);
}

// ===================================================================

class S_TEST::_Internal {
 public:
};

void S_TEST::clear_buffs() {
  _impl_.buffs_.Clear();
}
S_TEST::S_TEST(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::Message(arena, is_message_owned) {
  SharedCtor(arena, is_message_owned);
  // @@protoc_insertion_point(arena_constructor:Protocol.S_TEST)
}
S_TEST::S_TEST(const S_TEST& from)
  : ::PROTOBUF_NAMESPACE_ID::Message() {
  S_TEST* const _this = this; (void)_this;
  new (&_impl_) Impl_{
      decltype(_impl_.buffs_){from._impl_.buffs_}
    , decltype(_impl_.id_){}
    , decltype(_impl_.hp_){}
    , decltype(_impl_.attack_){}
    , /*decltype(_impl_._cached_size_)*/{}};

  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  ::memcpy(&_impl_.id_, &from._impl_.id_,
    static_cast<size_t>(reinterpret_cast<char*>(&_impl_.attack_) -
    reinterpret_cast<char*>(&_impl_.id_)) + sizeof(_impl_.attack_));
  // @@protoc_insertion_point(copy_constructor:Protocol.S_TEST)
}

inline void S_TEST::SharedCtor(
    ::_pb::Arena* arena, bool is_message_owned) {
  (void)arena;
  (void)is_message_owned;
  new (&_impl_) Impl_{
      decltype(_impl_.buffs_){arena}
    , decltype(_impl_.id_){uint64_t{0u}}
    , decltype(_impl_.hp_){0u}
    , decltype(_impl_.attack_){0u}
    , /*decltype(_impl_._cached_size_)*/{}
  };
}

S_TEST::~S_TEST() {
  // @@protoc_insertion_point(destructor:Protocol.S_TEST)
  if (auto *arena = _internal_metadata_.DeleteReturnArena<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>()) {
  (void)arena;
    return;
  }
  SharedDtor();
}

inline void S_TEST::SharedDtor() {
  GOOGLE_DCHECK(GetArenaForAllocation() == nullptr);
  _impl_.buffs_.~RepeatedPtrField();
}

void S_TEST::SetCachedSize(int size) const {
  _impl_._cached_size_.Set(size);
}

void S_TEST::Clear() {
// @@protoc_insertion_point(message_clear_start:Protocol.S_TEST)
  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  _impl_.buffs_.Clear();
  ::memset(&_impl_.id_, 0, static_cast<size_t>(
      reinterpret_cast<char*>(&_impl_.attack_) -
      reinterpret_cast<char*>(&_impl_.id_)) + sizeof(_impl_.attack_));
  _internal_metadata_.Clear<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>();
}

const char* S_TEST::_InternalParse(const char* ptr, ::_pbi::ParseContext* ctx) {
#define CHK_(x) if (PROTOBUF_PREDICT_FALSE(!(x))) goto failure
  while (!ctx->Done(&ptr)) {
    uint32_t tag;
    ptr = ::_pbi::ReadTag(ptr, &tag);
    switch (tag >> 3) {
      // uint64 id = 1;
      case 1:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 8)) {
          _impl_.id_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint64(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // uint32 hp = 2;
      case 2:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 16)) {
          _impl_.hp_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // uint32 attack = 3;
      case 3:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 24)) {
          _impl_.attack_ = ::PROTOBUF_NAMESPACE_ID::internal::ReadVarint32(&ptr);
          CHK_(ptr);
        } else
          goto handle_unusual;
        continue;
      // repeated .Protocol.BuffData buffs = 4;
      case 4:
        if (PROTOBUF_PREDICT_TRUE(static_cast<uint8_t>(tag) == 34)) {
          ptr -= 1;
          do {
            ptr += 1;
            ptr = ctx->ParseMessage(_internal_add_buffs(), ptr);
            CHK_(ptr);
            if (!ctx->DataAvailable(ptr)) break;
          } while (::PROTOBUF_NAMESPACE_ID::internal::ExpectTag<34>(ptr));
        } else
          goto handle_unusual;
        continue;
      default:
        goto handle_unusual;
    }  // switch
  handle_unusual:
    if ((tag == 0) || ((tag & 7) == 4)) {
      CHK_(ptr);
      ctx->SetLastTag(tag);
      goto message_done;
    }
    ptr = UnknownFieldParse(
        tag,
        _internal_metadata_.mutable_unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(),
        ptr, ctx);
    CHK_(ptr != nullptr);
  }  // while
message_done:
  return ptr;
failure:
  ptr = nullptr;
  goto message_done;
#undef CHK_
}

uint8_t* S_TEST::_InternalSerialize(
    uint8_t* target, ::PROTOBUF_NAMESPACE_ID::io::EpsCopyOutputStream* stream) const {
  // @@protoc_insertion_point(serialize_to_array_start:Protocol.S_TEST)
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  // uint64 id = 1;
  if (this->_internal_id() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteUInt64ToArray(1, this->_internal_id(), target);
  }

  // uint32 hp = 2;
  if (this->_internal_hp() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteUInt32ToArray(2, this->_internal_hp(), target);
  }

  // uint32 attack = 3;
  if (this->_internal_attack() != 0) {
    target = stream->EnsureSpace(target);
    target = ::_pbi::WireFormatLite::WriteUInt32ToArray(3, this->_internal_attack(), target);
  }

  // repeated .Protocol.BuffData buffs = 4;
  for (unsigned i = 0,
      n = static_cast<unsigned>(this->_internal_buffs_size()); i < n; i++) {
    const auto& repfield = this->_internal_buffs(i);
    target = ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::
        InternalWriteMessage(4, repfield, repfield.GetCachedSize(), target, stream);
  }

  if (PROTOBUF_PREDICT_FALSE(_internal_metadata_.have_unknown_fields())) {
    target = ::_pbi::WireFormat::InternalSerializeUnknownFieldsToArray(
        _internal_metadata_.unknown_fields<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(::PROTOBUF_NAMESPACE_ID::UnknownFieldSet::default_instance), target, stream);
  }
  // @@protoc_insertion_point(serialize_to_array_end:Protocol.S_TEST)
  return target;
}

size_t S_TEST::ByteSizeLong() const {
// @@protoc_insertion_point(message_byte_size_start:Protocol.S_TEST)
  size_t total_size = 0;

  uint32_t cached_has_bits = 0;
  // Prevent compiler warnings about cached_has_bits being unused
  (void) cached_has_bits;

  // repeated .Protocol.BuffData buffs = 4;
  total_size += 1UL * this->_internal_buffs_size();
  for (const auto& msg : this->_impl_.buffs_) {
    total_size +=
      ::PROTOBUF_NAMESPACE_ID::internal::WireFormatLite::MessageSize(msg);
  }

  // uint64 id = 1;
  if (this->_internal_id() != 0) {
    total_size += ::_pbi::WireFormatLite::UInt64SizePlusOne(this->_internal_id());
  }

  // uint32 hp = 2;
  if (this->_internal_hp() != 0) {
    total_size += ::_pbi::WireFormatLite::UInt32SizePlusOne(this->_internal_hp());
  }

  // uint32 attack = 3;
  if (this->_internal_attack() != 0) {
    total_size += ::_pbi::WireFormatLite::UInt32SizePlusOne(this->_internal_attack());
  }

  return MaybeComputeUnknownFieldsSize(total_size, &_impl_._cached_size_);
}

const ::PROTOBUF_NAMESPACE_ID::Message::ClassData S_TEST::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::Message::CopyWithSourceCheck,
    S_TEST::MergeImpl
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*S_TEST::GetClassData() const { return &_class_data_; }


void S_TEST::MergeImpl(::PROTOBUF_NAMESPACE_ID::Message& to_msg, const ::PROTOBUF_NAMESPACE_ID::Message& from_msg) {
  auto* const _this = static_cast<S_TEST*>(&to_msg);
  auto& from = static_cast<const S_TEST&>(from_msg);
  // @@protoc_insertion_point(class_specific_merge_from_start:Protocol.S_TEST)
  GOOGLE_DCHECK_NE(&from, _this);
  uint32_t cached_has_bits = 0;
  (void) cached_has_bits;

  _this->_impl_.buffs_.MergeFrom(from._impl_.buffs_);
  if (from._internal_id() != 0) {
    _this->_internal_set_id(from._internal_id());
  }
  if (from._internal_hp() != 0) {
    _this->_internal_set_hp(from._internal_hp());
  }
  if (from._internal_attack() != 0) {
    _this->_internal_set_attack(from._internal_attack());
  }
  _this->_internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
}

void S_TEST::CopyFrom(const S_TEST& from) {
// @@protoc_insertion_point(class_specific_copy_from_start:Protocol.S_TEST)
  if (&from == this) return;
  Clear();
  MergeFrom(from);
}

bool S_TEST::IsInitialized() const {
  return true;
}

void S_TEST::InternalSwap(S_TEST* other) {
  using std::swap;
  _internal_metadata_.InternalSwap(&other->_internal_metadata_);
  _impl_.buffs_.InternalSwap(&other->_impl_.buffs_);
  ::PROTOBUF_NAMESPACE_ID::internal::memswap<
      PROTOBUF_FIELD_OFFSET(S_TEST, _impl_.attack_)
      + sizeof(S_TEST::_impl_.attack_)
      - PROTOBUF_FIELD_OFFSET(S_TEST, _impl_.id_)>(
          reinterpret_cast<char*>(&_impl_.id_),
          reinterpret_cast<char*>(&other->_impl_.id_));
}

::PROTOBUF_NAMESPACE_ID::Metadata S_TEST::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_Protocol_2eproto_getter, &descriptor_table_Protocol_2eproto_once,
      file_level_metadata_Protocol_2eproto[1]);
}

// ===================================================================

class C_MOVE::_Internal {
 public:
};

C_MOVE::C_MOVE(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase(arena, is_message_owned) {
  // @@protoc_insertion_point(arena_constructor:Protocol.C_MOVE)
}
C_MOVE::C_MOVE(const C_MOVE& from)
  : ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase() {
  C_MOVE* const _this = this; (void)_this;
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:Protocol.C_MOVE)
}





const ::PROTOBUF_NAMESPACE_ID::Message::ClassData C_MOVE::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::CopyImpl,
    ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::MergeImpl,
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*C_MOVE::GetClassData() const { return &_class_data_; }







::PROTOBUF_NAMESPACE_ID::Metadata C_MOVE::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_Protocol_2eproto_getter, &descriptor_table_Protocol_2eproto_once,
      file_level_metadata_Protocol_2eproto[2]);
}

// ===================================================================

class S_LOGIN::_Internal {
 public:
};

S_LOGIN::S_LOGIN(::PROTOBUF_NAMESPACE_ID::Arena* arena,
                         bool is_message_owned)
  : ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase(arena, is_message_owned) {
  // @@protoc_insertion_point(arena_constructor:Protocol.S_LOGIN)
}
S_LOGIN::S_LOGIN(const S_LOGIN& from)
  : ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase() {
  S_LOGIN* const _this = this; (void)_this;
  _internal_metadata_.MergeFrom<::PROTOBUF_NAMESPACE_ID::UnknownFieldSet>(from._internal_metadata_);
  // @@protoc_insertion_point(copy_constructor:Protocol.S_LOGIN)
}





const ::PROTOBUF_NAMESPACE_ID::Message::ClassData S_LOGIN::_class_data_ = {
    ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::CopyImpl,
    ::PROTOBUF_NAMESPACE_ID::internal::ZeroFieldsBase::MergeImpl,
};
const ::PROTOBUF_NAMESPACE_ID::Message::ClassData*S_LOGIN::GetClassData() const { return &_class_data_; }







::PROTOBUF_NAMESPACE_ID::Metadata S_LOGIN::GetMetadata() const {
  return ::_pbi::AssignDescriptors(
      &descriptor_table_Protocol_2eproto_getter, &descriptor_table_Protocol_2eproto_once,
      file_level_metadata_Protocol_2eproto[3]);
}

// @@protoc_insertion_point(namespace_scope)
}  // namespace Protocol
PROTOBUF_NAMESPACE_OPEN
template<> PROTOBUF_NOINLINE ::Protocol::C_TEST*
Arena::CreateMaybeMessage< ::Protocol::C_TEST >(Arena* arena) {
  return Arena::CreateMessageInternal< ::Protocol::C_TEST >(arena);
}
template<> PROTOBUF_NOINLINE ::Protocol::S_TEST*
Arena::CreateMaybeMessage< ::Protocol::S_TEST >(Arena* arena) {
  return Arena::CreateMessageInternal< ::Protocol::S_TEST >(arena);
}
template<> PROTOBUF_NOINLINE ::Protocol::C_MOVE*
Arena::CreateMaybeMessage< ::Protocol::C_MOVE >(Arena* arena) {
  return Arena::CreateMessageInternal< ::Protocol::C_MOVE >(arena);
}
template<> PROTOBUF_NOINLINE ::Protocol::S_LOGIN*
Arena::CreateMaybeMessage< ::Protocol::S_LOGIN >(Arena* arena) {
  return Arena::CreateMessageInternal< ::Protocol::S_LOGIN >(arena);
}
PROTOBUF_NAMESPACE_CLOSE

// @@protoc_insertion_point(global_scope)
#include <google/protobuf/port_undef.inc>
