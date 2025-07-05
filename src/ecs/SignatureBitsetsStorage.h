//
// Created by Zéro Cool on 05/07/2025.
//

#ifndef SIGNATUREBITSETSSTORAGE_H
#define SIGNATUREBITSETSSTORAGE_H

#include <tuple>

#include "tools/for_each_type.h"

namespace ecs::impl {

    /**
     * Classe de stockage des bitset pour les signatures
     *
     * @tparam TSettings Paramétrage ECS
     */
    template<typename TSettings>
    class SignatureBitsetsStorage
    {
        // Settings = Settings<ComponentList, TagList, SignatureList>
        using Settings = TSettings;
        // SignatureBitsets = SignatureBitsets<
        //    Settings<
        //       TypeList<C0, C1, C2, ...>,
        //       TypeList<T0, T1, T2, ...>,
        //       TypeList<ecs:Signature<>, ecs:Signature<S0, S1>, ecs:Signature<S0, S3, ...>, ...>
        //    >
        // >
        using SignatureBitsets = typename Settings::SignatureBitsets;
        // SignatureList = TypeList<ecs::Signature<C0, C1>, ecs::Signature<C0, C3, ...>, ...>
        using SignatureList = typename SignatureBitsets::SignatureList;
        // BitsetStorage = TypeList<Bitset, Bitset, ...>>
        // BitsetStorage = TypeList<std::bitset<componentCount() + tagCount()>, std::bitset<componentCount() + tagCount()>, ...>>
        using BitsetStorage = typename SignatureBitsets::BitsetStorage;

        BitsetStorage storage;

    public:
        /**
         * Récupère le bitset correspondant à une signature donnée
         * @tparam TSignature Type de signature à utiliser pour récupérer son bitset
         * @return bitset de la signature correspondant
         */
        template<typename TSignature>
        auto &getSignatureBitset() noexcept {
            static_assert(Settings::template isSignature<TSignature>(), "");
            return std::get<Settings::template signatureID<TSignature>()>(storage);
        }

        /**
         * Récupère le bitset correspondant à une signature donnée (const)
         * @tparam TSignature Type de signature à utiliser pour récupérer son bitset
         * @return bitset de la signature correspondant (const)
         */
        template<typename TSignature>
        const auto& getSignatureBitset() const noexcept
        {
            static_assert(Settings::template isSignature<TSignature>(), "");
            return std::get<Settings::template signatureID<TSignature>()>(storage);
        }

    private:
        /**
         * Méthode permettant d'initialiser le bitset correspondant à la signature donnée.
         *
         * @tparam TSignature Signature correspondant au bitset à initialiser
         */
        template<typename TSignature>
        void initializeBitset() noexcept
        {
            // NOTE : For Debug... All is OK
            // std::cout << "initializeBitset for " << typeid(TSignature).name() << std::endl;

            auto& b(this->getSignatureBitset<TSignature>());

            using SignatureComponents = typename SignatureBitsets::template SignatureComponents<TSignature>;

            using SignatureTags = typename SignatureBitsets::template SignatureTags<TSignature>;

            //forTypes<SignatureComponents>([this, &b](auto t) {
            tools::for_each_type<SignatureComponents>([this, &b]<typename U>() {
                // NOTE : For Debug... All is OK
                // std::cout << "   set for component " << typeid(U).name() << " b[" << Settings::template componentBit<U>() << "] = true" << std::endl;
                b[Settings::template componentBit<U>()] = true;
            });

            //forTypes<SignatureTags>([this, &b](auto t) {
            tools::for_each_type<SignatureTags>([this, &b]<typename U>() {
                // NOTE : For Debug... All is OK
                // std::cout << "   set for tag " << typeid(U).name() << " b[" << Settings::template tagBit<U>() << "] = true" << std::endl;
                b[Settings::template tagBit<U>()] = true;
            });

            // NOTE : For Debug... All is OK
            // std::cout << "   b = " << b << std::endl;
        }

    public:
        /**
         * Constructeur initialsant tous les bitsets de toutes les signatures
         */
        SignatureBitsetsStorage() noexcept {
            //forTypes<SignatureList>([this](auto t) {
            tools::for_each_type<SignatureList>([this]<typename TSignature>() {
                // t doit être de type Signature...
                //this->initializeBitset<typename decltype(t)::type>();

                // NOTE : For Debug... All is OK
                //std::cout << "SignatureBitsetsStorage for " << typeid(T).name() << std::endl;
                this->initializeBitset<TSignature>();
            });
        }
    };

}

#endif //SIGNATUREBITSETSSTORAGE_H
