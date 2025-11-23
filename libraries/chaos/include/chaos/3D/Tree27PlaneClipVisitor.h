namespace chaos
{
#ifdef CHAOS_FORWARD_DECLARATION

	class Tree27PlaneClipVisitor;

#elif !defined CHAOS_TEMPLATE_IMPLEMENTATION

	// Acceptance criteria: a node is accepted if it's (even partially) inside the negative section of space 
	//
	// 
	// A                    negative   /  positive
	//  +---------------+   section   /   section
	//  |               |   of       /    of
	//  |               |   space   /     space
	//  |      pos      |          /
	//  |       +       |         +__
	//  |         \     |        /    --> Normal
	//  | half_size \   |       /         
	//  |             \ |      /
	//  +---------------+     /
	//                   B   /
	//                      /
	//                     /
	//
	// if A is in positive part, the whole node and its hierarchy is to be rejected
	// 
	// if B is in negative part, then the whole square is inside the negative part -> the tree27 node and all its children are full inside the negative part of space
	// -> no need to test this plane for rejection anymore, every node are to be accepted
	//

	class Tree27PlaneClipVisitor
	{
	public:

		/** entry point of the visit algorithm */
		template<bool DEPTH_FIRST = false, typename LOOSE_TREE27_TYPE, typename FUNC>
		static auto Visit(
			LOOSE_TREE27_TYPE& tree27,
			typename LOOSE_TREE27_TYPE::plane_type const* in_planes,
			size_t in_plane_count,
			FUNC const& in_func) -> meta::LambdaInfo<FUNC, decltype(tree27.GetRootNode()), typename LOOSE_TREE27_TYPE::plane_type const*, size_t, uint32_t>::result_type
		{
			Tree27PlaneClipVisitorImpl<LOOSE_TREE27_TYPE::dimension, typename LOOSE_TREE27_TYPE::type, DEPTH_FIRST, FUNC> impl(in_planes, in_plane_count, in_func);
			return impl.Visit(tree27);
		}

	protected:

		CHAOS_GEOMETRY_TEMPLATE(DIMENSION, T, bool DEPTH_FIRST, typename FUNC)
		class Tree27PlaneClipVisitorImpl
		{
			using geometry = type_geometric<DIMENSION, T>;

			/** the type for vector */
			using vec_type = geometry::vec_type;
			/** the type for plane */
			using plane_type = typename geometry::plane_type;
			/** the type for box */
			using box_type = typename geometry::box_type;

		public:

			/** constructor */
			Tree27PlaneClipVisitorImpl(plane_type const* in_planes, size_t in_plane_count, FUNC const& in_func) :
				planes(in_planes),
				plane_count(in_plane_count),
				func(in_func)
			{
				assert(planes != nullptr);
				assert(plane_count > 0 && plane_count < 32);
			}

			/** entry point of the visit algorithm */
			template<typename LOOSE_TREE27_TYPE>
			auto Visit(LOOSE_TREE27_TYPE& tree27) -> meta::LambdaInfo<FUNC, decltype(tree27.GetRootNode()), plane_type const*, size_t, uint32_t>::result_type
			{
				using L = meta::LambdaInfo<FUNC, decltype(tree27.GetRootNode()), plane_type const*, size_t, uint32_t>;

				if (tree27.GetRootNode() == nullptr)
				{
					if constexpr (L::convertible_to_bool)
						return typename L::result_type{};
				}
				else
				{
					uint32_t plane_bitfield = (1 << uint32_t(plane_count)) - 1;
					return Visit(tree27.GetRootNode(), plane_bitfield);
				}
			}

		protected:

			template<typename NODE27_TYPE>
			auto Visit(NODE27_TYPE* node, uint32_t plane_bitfield) -> meta::LambdaInfo<FUNC, NODE27_TYPE*, plane_type const*, size_t, uint32_t>::result_type
			{
				using L = meta::LambdaInfo<FUNC, NODE27_TYPE*, plane_type const*, size_t, uint32_t>;

				assert(node != nullptr);
				assert(plane_bitfield != 0);

				box_type box = node->GetBoundingBox();

				bool node_rejected = BitTools::ForEachBitForward(plane_bitfield, [&](uint32_t index)
					{
						plane_type const& plane = planes[index];

						// compute A & B (see schema above)
						auto AsUint32 = [](float src)
						{
							return *(uint32_t*)(&src);
						};
						auto AsFloat = [](uint32_t src)
						{
							return *(float*)(&src);
						};

						vec_type directed_half_size;
						for (size_t i = 0; i < DIMENSION; ++i)
						{
							uint32_t normal_direction_sign = AsUint32(plane[i]) & (1 << 31);                       // just keep the sign
							uint32_t half_size_component = AsUint32(box.half_size[i]) & ~(1 << 31);              // remove the sign from half_size
							directed_half_size[i] = AsFloat(half_size_component | normal_direction_sign); // add to half sign the sign coming from the plane normal
						}

						vec_type A = box.position - directed_half_size;
						float section_A = glm::dot({ A, 1.0f }, plane);
						if (section_A > 0.0f)
							return true; // stop processing (whole node & hierarchy is rejected)

						vec_type B = box.position + directed_half_size;
						float section_B = glm::dot({ B, 1.0f }, plane);
						if (section_B < 0.0f)
							plane_bitfield &= ~(1 << index); // this plane does need to be processed ever anymore
						return false; // continue processing
					});

				if (node_rejected)
				{
					if constexpr (L::convertible_to_bool)
						return typename L::result_type{};
				}
				else
				{
					if (plane_bitfield == 0)
					{
						return node->Visit<DEPTH_FIRST>([&](auto* node) // no need to bother with planes anymore
						{
							return func(node, planes, plane_count, plane_bitfield);
						});
					}
					else
					{
						for (int i = 0; i < 2; ++i)
						{
							if ((i == 0 && DEPTH_FIRST) || (i == 1 && !DEPTH_FIRST))
							{
								if constexpr (L::convertible_to_bool)
								{
									decltype(auto) result = node->ForEachChildren([&](auto* node)
									{
										return Visit(node, plane_bitfield);
									});
									if (result)
										return result;
								}
								else
								{
									node->ForEachChildren([&](auto* node)
									{
										Visit(node, plane_bitfield);
									});
								}
							}
							else
							{
								if constexpr (L::convertible_to_bool)
								{
									decltype(auto) result = func(node, planes, plane_count, plane_bitfield);
									if (result)
										return result;
								}
								else
								{
									func(node, planes, plane_count, plane_bitfield);
								}
							}
						}
					}
				}
			}

		protected:

			/** the planes to consider */
			plane_type const* planes = nullptr;
			/** number of planes */
			size_t plane_count = 0;
			/** functor to call on each node */
			FUNC const& func;
		};
	};


#endif

}; // namespace chaos