#pragma once

namespace testApp
{
    struct ProjectDataContext: public dory::domain::DataContext
    {
        dory::domain::entity::IdType inputGroupNodeId;
        dory::domain::entity::IdType outputGroupNodeId;
    };
}