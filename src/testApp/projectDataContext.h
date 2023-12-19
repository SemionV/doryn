#pragma once

namespace testApp
{
    struct ProjectDataContext: public dory::domain::DataContext
    {
        dory::domain::entity::IdType inputGroupNodeId;
        dory::domain::entity::IdType outputGroupNodeId;
        dory::domain::entity::IdType mainWindowId;

        ProjectDataContext():
            inputGroupNodeId(dory::entity::nullId),
            outputGroupNodeId(dory::entity::nullId),
            mainWindowId(dory::entity::nullId)
        {}
    };
}