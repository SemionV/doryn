#pragma once

namespace testApp
{
    struct ProjectDataContext: public dory::domain::DataContext
    {
        dory::domain::entity::IdType inputGroupNodeId;
        dory::domain::entity::IdType outputGroupNodeId;
        dory::domain::entity::IdType mainWindowId;

        ProjectDataContext():
            inputGroupNodeId(dory::domain::entity::nullId),
            outputGroupNodeId(dory::domain::entity::nullId),
            mainWindowId(dory::domain::entity::nullId)
        {}
    };
}