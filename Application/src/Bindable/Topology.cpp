#include "Topology.h"

Topology::Topology(Graphics& gfx, D3D11_PRIMITIVE_TOPOLOGY topology)
    : m_Topology(topology)
{}

void Topology::Bind(Graphics& gfx) noexcept
{
    GetContext(gfx)->IASetPrimitiveTopology(m_Topology);
}
