/*
 * Copyright (c) Simverge Software LLC - All Rights Reserved
 */

#include "pdalc_pipeline.h"
#include "pdalc_pointviewiterator.h"

#include <pdal/PipelineExecutor.hpp>

#include <string>

namespace pdal
{
	namespace capi
	{
		extern "C"
		{
			PDALPipelinePtr PDALCreatePipeline(const char* json)
			{
				PDALPipelinePtr pipeline = nullptr;

				if (json && std::strlen(json) > 0)
				{
					pdal::PipelineExecutor *executor = nullptr;
					
					try
					{
						pdal::PipelineExecutor stackpipe(json);
						executor = new pdal::PipelineExecutor(json);
					}
					catch (const std::exception &e)
					{
						printf("Could not create pipeline: %s\n%s\n", e.what(), json);
						executor = nullptr;
					}

					if (executor)
					{
						bool valid = false;

						try
						{
							valid = executor->validate();
						}
						catch (const std::exception &e)
						{
							printf("Error while validating pipeline: %s\n%s\n", e.what(), json);
						}

						if (valid)
						{
							pipeline = new Pipeline(executor);
						}
						else
						{
							delete executor;
							executor = NULL;
							printf("The pipeline is invalid:\n%s\n", json);
						}
					}
				}

				return pipeline;
			}

			void PDALDisposePipeline(PDALPipelinePtr pipeline)
			{
				if (pipeline)
				{
					Pipeline *ptr = reinterpret_cast<Pipeline *>(pipeline);
					delete ptr;
				}
			}

			size_t PDALGetPipelineAsString(PDALPipelinePtr pipeline, char *buffer, size_t size)
			{
				size_t result = 0;

				if (pipeline && buffer && size > 0)
				{
					Pipeline *ptr = reinterpret_cast<Pipeline *>(pipeline);
					pdal::PipelineExecutor *executor = ptr->get();
					buffer[0] =  '\0';
					buffer[size - 1] = '\0';

					if (executor)
					{
						try
						{
							std::string s = executor->getPipeline();
							std::strncpy(buffer, s.c_str(), size - 1);
							result = std::min(s.length(), size);
						}
						catch (const std::exception &e)
						{
							printf("Found error while retrieving pipeline's string representation: %s\n", e.what());
						}
					}

				}

				return result;
			}

			size_t PDALGetPipelineMetadata(PDALPipelinePtr pipeline, char *metadata, size_t size)
			{
				size_t result = 0;

				if (pipeline && metadata && size > 0)
				{
					Pipeline *ptr = reinterpret_cast<Pipeline *>(pipeline);
					pdal::PipelineExecutor *executor = ptr->get();
					metadata[0] =  '\0';
					metadata[size - 1] = '\0';

					if (executor)
					{
						try
						{
							std::string s = executor->getMetadata();
							std::strncpy(metadata, s.c_str(), size);
							result = std::min(s.length(), size);
						}
						catch (const std::exception &e)
						{
							printf("Found error while retrieving pipeline's metadata: %s\n", e.what());
						}
				}
				}

				return result;
			}

			size_t PDALGetPipelineSchema(PDALPipelinePtr pipeline, char *schema, size_t size)
			{
				size_t result = 0;

				if (pipeline && schema && size > 0)
				{
					Pipeline *ptr = reinterpret_cast<Pipeline *>(pipeline);
					pdal::PipelineExecutor *executor = ptr->get();
					schema[0] =  '\0';
					schema[size - 1] = '\0';

					if (executor)
					{
						try
						{
							std::string s = executor->getSchema();
							std::strncpy(schema, s.c_str(), size);
							result = std::min(s.length(), size);
						}
						catch (const std::exception &e)
						{
							printf("Found error while retrieving pipeline's schema: %s\n", e.what());
						}
					}
				}

				return result;
			}

			size_t PDALGetPipelineLog(PDALPipelinePtr pipeline, char *log, size_t size)
			{
				size_t result = 0;

				if (pipeline && log && size > 0)
				{
					Pipeline *ptr = reinterpret_cast<Pipeline *>(pipeline);
					pdal::PipelineExecutor *executor = ptr->get();
					log[0] =  '\0';
					log[size - 1] = '\0';

					if (executor)
					{
						try
						{
						std::string s = executor->getLog();
						std::strncpy(log, s.c_str(), size);
						result = std::min(s.length(), size);
					}
						catch (const std::exception &e)
						{
							printf("Found error while retrieving pipeline's log: %s\n", e.what());
						}
					}
				}

				return result;
			}

			void PDALSetPipelineLogLevel(PDALPipelinePtr pipeline, int level)
			{
				Pipeline *ptr = reinterpret_cast<Pipeline *>(pipeline);

				if (ptr && ptr->get())
				{
					try
					{
					ptr->get()->setLogLevel(level);
				}
					catch (const std::exception &e)
					{
						printf("Found error while setting log level: %s\n", e.what());
					}
				}
			}

			int PDALGetPipelineLogLevel(PDALPipelinePtr pipeline)
			{
				Pipeline *ptr = reinterpret_cast<Pipeline *>(pipeline);
				return (ptr && ptr->get()) ? ptr->get()->getLogLevel() : 0;
			}

			int64_t PDALExecutePipeline(PDALPipelinePtr pipeline)
			{
				int64_t result = 0;
				Pipeline *ptr = reinterpret_cast<Pipeline *>(pipeline);

				if (ptr && ptr->get())
				{
					try
					{
						result = ptr->get()->execute();
			}
					catch (const std::exception &e)
					{
						printf("Found error while executing pipeline: %s", e.what());
					}
				}

				return result;
			}

			bool PDALValidatePipeline(PDALPipelinePtr pipeline)
			{
				int64_t result = 0;
				Pipeline *ptr = reinterpret_cast<Pipeline *>(pipeline);

				if (ptr && ptr->get())
				{
					try
					{
						result = ptr->get()->validate();
					}
					catch (const std::exception &e)
					{
						printf("Found error while validating pipeline: %s", e.what());
					}
				}

				return result;
			}

			PDALPointViewIteratorPtr PDALGetPointViews(PDALPipelinePtr pipeline)
			{
				Pipeline *ptr = reinterpret_cast<Pipeline *>(pipeline);
				pdal::capi::PointViewIterator *views = nullptr;
				
				if (ptr && ptr->get())
				{
					try
					{
						auto &v = ptr->get()->getManagerConst().views();

						if (!v.empty())
						{
							views = new pdal::capi::PointViewIterator(v);
						}
					}
					catch (const std::exception &e)
					{
						printf("Found error while retrieving point views: %s\n", e.what());
					}
				}

				return views;
			}
		}
	}
}
