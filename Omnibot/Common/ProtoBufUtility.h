#pragma once

#ifndef _PROTOBUFUTILITY_H_
#define _PROTOBUFUTILITY_H_

namespace google
{
	namespace protobuf
	{
		class Message;
		class FieldDescriptor;
	};
};

#include <string>
#include <vector>

std::string GetFieldString( const google::protobuf::Message & msg, const google::protobuf::FieldDescriptor * fieldDesc );
void ClearDefaultedValues( google::protobuf::Message & msg, size_t & optionalFieldsSet );

#endif