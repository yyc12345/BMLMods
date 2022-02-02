#include "main.h"

IMod* BMLEntry(IBML* bml) {
	return new Coredump(bml);
}

void Coredump::OnLoad() {
	m_bml->RegisterCommand(new CoredumpCommand());
}

void CoredumpCommand::Execute(IBML* bml, const std::vector<std::string>& args) {
	CKContext* ctx = bml->GetCKContext();
	CKObjectArray* finalArray = CreateCKObjectArray();
	const XObjectPointerArray* tmpArray = &(ctx->GetObjectListByType(CKCID_BEHAVIOR, TRUE));
	int count = tmpArray->Size();
	for (int i = 0; i < count; i++)
		finalArray->AddIfNotHere(tmpArray->GetObjectA(i));
	
	CKDependencies dep;
	dep.m_Flags = CK_DEPENDENCIES_FULL;
	CKERROR code = ctx->Save("runtime_context.nmo", finalArray, 0xFFFFFFFF, &dep);
	DeleteCKObjectArray(finalArray);

	if (code == CK_OK) bml->SendIngameMessage("Coredump successfully!");
	else bml->SendIngameMessage("Fail in Coredumping!");
}

const std::vector<std::string> CoredumpCommand::GetTabCompletion(IBML* bml, const std::vector<std::string>& args) {
	return std::vector<std::string>{};
}
