#include "QtMemoryViewModelTests.h"
#include <memory>
#include "../QtMemoryViewModel.h"

void CQtMemoryViewModelTests::RunTests()
{
	m_getByteFunction = [this](uint32 address) { return GetByte(address); };
	TestSimpleBytes();
	TestOutOfBounds();
	TestAddressToModelIndex();
	TestAddressToModelIndexWord();
	TestModelIndexToAddress();
	TestModelIndexToAddressWord();
}

uint8 CQtMemoryViewModelTests::GetByte(uint32 address)
{
	return m_memory[address];
}

void CQtMemoryViewModelTests::TestSimpleBytes()
{
	uint32 memorySize = 0x100000;
	m_memory.clear();
	m_memory.resize(memorySize);
	for(unsigned int i = 0; i < memorySize; i++)
	{
		m_memory[i] = static_cast<uint8>(i);
	}

	auto model = std::make_unique<CQtMemoryViewModel>(nullptr);
	model->SetData(m_getByteFunction, memorySize);
	model->SetBytesPerRow(0x10);

	TEST_VERIFY(model->data(model->index(1, 0)) == "10");
	TEST_VERIFY(model->data(model->index(2, 0)) == "20");
}

void CQtMemoryViewModelTests::TestOutOfBounds()
{
	uint32 memorySize = 0x18;
	m_memory.clear();
	m_memory.resize(memorySize);

	auto model = std::make_unique<CQtMemoryViewModel>(nullptr);
	model->SetData(m_getByteFunction, memorySize);
	model->SetBytesPerRow(0x10);

	uint32 rowCount = model->rowCount();
	TEST_VERIFY(rowCount == 2);

	TEST_VERIFY(model->data(model->index(1, 0x00)) == "00");
	TEST_VERIFY(model->data(model->index(1, 0x08)) == "--");
	TEST_VERIFY(model->data(model->index(1, 0x10)) == "........        ");
}

void CQtMemoryViewModelTests::TestAddressToModelIndex()
{
	uint32 memorySize = 256;
	m_memory.clear();
	m_memory.resize(memorySize);

	uint32 bytesPerRow = 21;

	auto model = std::make_unique<CQtMemoryViewModel>(nullptr);
	model->SetData(m_getByteFunction, memorySize);
	model->SetBytesPerRow(bytesPerRow);

	//ceil(256 / 21) = 13
	TEST_VERIFY(model->rowCount() == 13);

	//Some specific item in our list
	{
		auto modelIndex = model->TranslateAddressToModelIndex((bytesPerRow * 7) + 4);
		TEST_VERIFY(modelIndex.row() == 7);
		TEST_VERIFY(modelIndex.column() == 4);
	}

	//Last item in our list
	{
		auto modelIndex = model->TranslateAddressToModelIndex(memorySize - 1);
		TEST_VERIFY(modelIndex.row() == 12);
		TEST_VERIFY(modelIndex.column() == 3);
	}

	//An item below
	{
		auto modelIndex = model->TranslateAddressToModelIndex(-1);
		TEST_VERIFY(modelIndex.row() == 0);
		TEST_VERIFY(modelIndex.column() == 0);
	}

	//An item beyond
	{
		auto modelIndex = model->TranslateAddressToModelIndex(memorySize * 2);
		TEST_VERIFY(modelIndex == model->TranslateAddressToModelIndex(memorySize - 1));
	}
}

void CQtMemoryViewModelTests::TestAddressToModelIndexWord()
{
	uint32 memorySize = 512;
	m_memory.clear();
	m_memory.resize(memorySize);

	uint32 bytesPerRow = 12;

	auto model = std::make_unique<CQtMemoryViewModel>(nullptr);
	model->SetData(m_getByteFunction, memorySize);
	model->SetBytesPerRow(bytesPerRow);
	model->SetActiveUnit(1);

	TEST_VERIFY(model->GetBytesPerUnit() == 4);

	//ceil(512 / 12) = 11
	TEST_VERIFY(model->rowCount() == 43);

	//Some specific item in our list
	{
		auto modelIndex = model->TranslateAddressToModelIndex((bytesPerRow * 7) + 4);
		TEST_VERIFY(modelIndex.row() == 7);
		TEST_VERIFY(modelIndex.column() == 1);
	}

	//Some specific item in our list (unaligned)
	{
		auto modelIndex = model->TranslateAddressToModelIndex((bytesPerRow * 7) + 3);
		TEST_VERIFY(modelIndex.row() == 7);
		TEST_VERIFY(modelIndex.column() == 0);
	}

	//Last item in our list
	{
		auto modelIndex = model->TranslateAddressToModelIndex(memorySize - 1);
		TEST_VERIFY(modelIndex.row() == 42);
		TEST_VERIFY(modelIndex.column() == 1);
	}

	//An item below
	{
		auto modelIndex = model->TranslateAddressToModelIndex(-1);
		TEST_VERIFY(modelIndex.row() == 0);
		TEST_VERIFY(modelIndex.column() == 0);
	}

	//An item beyond
	{
		auto modelIndex = model->TranslateAddressToModelIndex(memorySize * 2);
		TEST_VERIFY(modelIndex == model->TranslateAddressToModelIndex(memorySize - 1));
	}
}

void CQtMemoryViewModelTests::TestModelIndexToAddress()
{
	uint32 memorySize = 1024;
	m_memory.clear();
	m_memory.resize(memorySize);

	uint32 bytesPerRow = 32;

	auto model = std::make_unique<CQtMemoryViewModel>(nullptr);
	model->SetData(m_getByteFunction, memorySize);
	model->SetBytesPerRow(bytesPerRow);

	TEST_VERIFY(model->TranslateModelIndexToAddress(model->index(0, 0)) == 0);
	TEST_VERIFY(model->TranslateModelIndexToAddress(model->index(16, 16)) == 528);
}

void CQtMemoryViewModelTests::TestModelIndexToAddressWord()
{
	uint32 memorySize = 512;
	m_memory.clear();
	m_memory.resize(memorySize);

	uint32 bytesPerRow = 16;

	auto model = std::make_unique<CQtMemoryViewModel>(nullptr);
	model->SetData(m_getByteFunction, memorySize);
	model->SetBytesPerRow(bytesPerRow);
	model->SetActiveUnit(1);

	TEST_VERIFY(model->GetBytesPerUnit() == 4);

	TEST_VERIFY(model->TranslateModelIndexToAddress(model->index(0, 0)) == 0);
	TEST_VERIFY(model->TranslateModelIndexToAddress(model->index(16, 3)) == 268);
}
