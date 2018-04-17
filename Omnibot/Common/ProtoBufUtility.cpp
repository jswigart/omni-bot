#include "ProtoBufUtility.h"

#include <memory>

#include <google/protobuf/message.h>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>

using namespace google;

std::string GetFieldString( const protobuf::Message & msg, const protobuf::FieldDescriptor * fieldDesc )
{
	using namespace google;

	try
	{
		switch( fieldDesc->type() )
		{
		case protobuf::FieldDescriptorProto_Type_TYPE_DOUBLE:
			{
				return std::to_string( msg.GetReflection()->GetDouble( msg, fieldDesc ) );
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_FLOAT:
			return std::to_string( msg.GetReflection()->GetFloat( msg, fieldDesc ) );
		case protobuf::FieldDescriptorProto_Type_TYPE_INT64:
		case protobuf::FieldDescriptorProto_Type_TYPE_SFIXED64:
		case protobuf::FieldDescriptorProto_Type_TYPE_SINT64:
			return std::to_string( msg.GetReflection()->GetInt64( msg, fieldDesc ) );
		case protobuf::FieldDescriptorProto_Type_TYPE_UINT64:
		case protobuf::FieldDescriptorProto_Type_TYPE_FIXED64:
			return std::to_string( msg.GetReflection()->GetUInt64( msg, fieldDesc ) );
		case protobuf::FieldDescriptorProto_Type_TYPE_INT32:
		case protobuf::FieldDescriptorProto_Type_TYPE_SFIXED32:
		case protobuf::FieldDescriptorProto_Type_TYPE_SINT32:
			return std::to_string( msg.GetReflection()->GetInt32( msg, fieldDesc ) );
		case protobuf::FieldDescriptorProto_Type_TYPE_BOOL:
			return std::to_string( msg.GetReflection()->GetBool( msg, fieldDesc ) );
		case protobuf::FieldDescriptorProto_Type_TYPE_UINT32:
		case protobuf::FieldDescriptorProto_Type_TYPE_FIXED32:
			return std::to_string( msg.GetReflection()->GetUInt32( msg, fieldDesc ) );
		case protobuf::FieldDescriptorProto_Type_TYPE_STRING:
			return msg.GetReflection()->GetString( msg, fieldDesc );
		case protobuf::FieldDescriptorProto_Type_TYPE_GROUP:
			return "GROUP";
		case protobuf::FieldDescriptorProto_Type_TYPE_MESSAGE:
			return "MESSAGE";
		case protobuf::FieldDescriptorProto_Type_TYPE_BYTES:
			return "BYTES";
		case protobuf::FieldDescriptorProto_Type_TYPE_ENUM:
			{
				const protobuf::EnumValueDescriptor * enumVal = msg.GetReflection()->GetEnum( msg, fieldDesc );
				return enumVal->name();
			}
		}
	}
	catch ( const std::exception & ex )
	{
		ex;
		/*EngineFuncs::ConsoleError(
		va( "Can't convert '%s' to appropriate type %s",
		args.at( 2 ).c_str(),
		protobuf::FieldDescriptor::kTypeToName[ fieldDesc->type() ] ) );*/
	}
	return "";
}

size_t ClearDefaultedValues( protobuf::Message & msg )
{
	size_t optionalFieldsSet = 0;

	using namespace google;
	const protobuf::Reflection * refl = msg.GetReflection();

	std::vector<const protobuf::FieldDescriptor*> fields;
	refl->ListFields( msg, &fields );

	if ( fields.size() == 0 )
		return 0;

	optionalFieldsSet = 0;

	std::unique_ptr<protobuf::Message> defaultMsg( msg.New() );

	const protobuf::Reflection * reflDefault = defaultMsg->GetReflection();
	
	for ( size_t i = 0; i < fields.size(); ++i )
	{
		const protobuf::FieldDescriptor * fieldDesc = fields[ i ];

		if ( fieldDesc->is_repeated() || fieldDesc->is_required() )
			continue;
		
		switch( fieldDesc->type() )
		{
		case protobuf::FieldDescriptorProto_Type_TYPE_DOUBLE:
			{
				if ( refl->GetDouble( msg, fieldDesc ) == reflDefault->GetDouble( *defaultMsg.get(), fieldDesc ) )
					refl->ClearField( &msg, fieldDesc );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_FLOAT:
			{
				if ( refl->GetFloat( msg, fieldDesc ) == reflDefault->GetFloat( *defaultMsg.get(), fieldDesc ) )
					refl->ClearField( &msg, fieldDesc );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_INT64:
		case protobuf::FieldDescriptorProto_Type_TYPE_SFIXED64:
		case protobuf::FieldDescriptorProto_Type_TYPE_SINT64:
			{
				if ( refl->GetInt64( msg, fieldDesc ) == reflDefault->GetInt64( *defaultMsg.get(), fieldDesc ) )
					refl->ClearField( &msg, fieldDesc );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_UINT64:
		case protobuf::FieldDescriptorProto_Type_TYPE_FIXED64:
			{
				if ( refl->GetUInt64( msg, fieldDesc ) == reflDefault->GetUInt64( *defaultMsg.get(), fieldDesc ) )
					refl->ClearField( &msg, fieldDesc );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_INT32:
		case protobuf::FieldDescriptorProto_Type_TYPE_SFIXED32:
		case protobuf::FieldDescriptorProto_Type_TYPE_SINT32:
			{
				if ( refl->GetInt32( msg, fieldDesc ) == reflDefault->GetInt32( *defaultMsg.get(), fieldDesc ) )
					refl->ClearField( &msg, fieldDesc );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_BOOL:
			{
				if ( refl->GetBool( msg, fieldDesc ) == reflDefault->GetBool( *defaultMsg.get(), fieldDesc ) )
					refl->ClearField( &msg, fieldDesc );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_UINT32:
		case protobuf::FieldDescriptorProto_Type_TYPE_FIXED32:
			{
				if ( refl->GetUInt32( msg, fieldDesc ) == reflDefault->GetUInt32( *defaultMsg.get(), fieldDesc ) )
					refl->ClearField( &msg, fieldDesc );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_STRING:
			{
				if ( refl->GetString( msg, fieldDesc ) == reflDefault->GetString( *defaultMsg.get(), fieldDesc ) )
					refl->ClearField( &msg, fieldDesc );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_MESSAGE:
			{
				protobuf::Message * subMsg = refl->MutableMessage( &msg, fieldDesc );

				ClearDefaultedValues( *subMsg );
				break;
			}
		case protobuf::FieldDescriptorProto_Type_TYPE_ENUM:
			{
				if ( refl->GetEnum( msg, fieldDesc ) == reflDefault->GetEnum( *defaultMsg.get(), fieldDesc ) )
					refl->ClearField( &msg, fieldDesc );
				break;
			}
		}

		if ( refl->HasField( msg, fieldDesc ) )
			++optionalFieldsSet;
	}
	return optionalFieldsSet;
}
